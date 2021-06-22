#include "mySocket.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sstream>

using namespace std;

char* trim( char*s) {
	while(*s && isspace(*s)) s++;
	int i = strlen(s);
	for(--i; i>=0; i--) if( !isspace(s[i]) ) break;
	s[i+1] = '\0';
	return s;
}

void Request::extract_key_value(char*s) {
	char* key = trim( strtok(s, ":")) ;
	char* val = strtok(NULL, "\r");
	if(strcmp(key, "Set-Cookie")==0 ) header["Cookie"] += string(val);
	else header[key] = string(val);
}

static int get_ip_port_path(const char* url,
  char**ip,
  char**port,
  char**path ) {
	const char *s=url;
	if(strncmp(url, "http://", strlen("http://")) == 0) {
		s+= strlen("http://");
	} else if(strncmp(url, "https://", strlen("https://")) == 0) {
		s+= strlen("https://");
	} 
	int n = strlen(s);
	int i=0;
	while( i<n && s[i]!='/' && s[i]!=':') i++;
	*ip = (char*) calloc(i+1,1);
	strncpy(*ip, s, i);
	if( i>=n ) { // no path, no port
		*port = (char*) malloc(20);
		strcpy(*port, "80");
		*path = (char*) malloc(20);
		strcpy(*path, "/");
		return 0;
	}

	if(s[i]==':') {
		i++;
		int j=i;
		while(i<n && s[i]!='/') i++;
		*port = (char*) calloc(i-j+1,1);
		strncpy(*port, s+j, i-j);
	} else {
		*port = (char*) malloc(20);
		sprintf(*port, "80");
	}

	if(i>=n) {
		*path = (char*) malloc(20);
		strcpy(*path, "/");
	} else {
		*path = (char*) calloc( strlen(s+i)+1, 1 );
		strcpy(*path, s+i);
	}

	return 0;
}

static void get_walltime(double* wc) {
	struct timeval tp;
	gettimeofday(&tp, NULL);
	*wc = (double) (tp.tv_sec + tp.tv_usec*1.0e-6);
}

mySocket::mySocket(const char* ipstr, const char* portstr) {
	struct addrinfo hints,*res;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	if ( getaddrinfo(ipstr, portstr, &hints, &res)!=0 ) {
		printf("Error for in getaddrinfo");
		exit(-1);
	}
	// use the first address to connect the socket
	sock = socket(AF_INET, SOCK_STREAM, 0);
	int flag = connect(sock, res->ai_addr, sizeof(struct sockaddr));
	if( flag== -1 ) {
		printf("Error in connect to socket");
		exit(-1);
	}
	printf("Connected!\n");
}

// 读之前需要很确信是有一个完整行的, it can be used in HTTP protocol
char* mySocket::readLine() {
	char* data, *cursor;
	int data_len=0, alloc_len=1024;
	data = (char*) malloc(alloc_len);
	while(1) {
		cursor = data+data_len;
		int rz = recv(sock, cursor, 1, 0);
		if(*cursor == '\n') break;
		else {
			data_len++;
			if(data_len+1>=alloc_len) {
				alloc_len<<=1;
				data = (char*) realloc(data, alloc_len);
			}
		}
	}
	// remove \r\n
	while( data_len>0 && (data[data_len-1]=='\r' || data[data_len-1]=='\n') )
		data_len--;
	data[data_len] = '\0';
	data = (char*) realloc(data, data_len+1);
	return data;
}

void * mySocket::readAsLongAsICan(int *len) {
	// return when time out, that is the no message duration is greater than tol
	void* data, *cursor;
	int data_len = 0, alloc_len = 1024;
	data = malloc( alloc_len);
	double tol = 2.0;
	double pre, now;
	int rz = recv(sock, data, alloc_len, 0);
	get_walltime( &pre );
	while(1) {
		if( rz > 0) {
			data_len += rz;
			if( data_len >= alloc_len ) {
				alloc_len <<= 1;
				data = realloc( data, alloc_len );
			}
			get_walltime( &pre );
		} else {
			get_walltime( &now);
			if( now - pre > tol ) break;
		}
		rz = recv(sock, data+data_len, alloc_len - data_len, MSG_DONTWAIT);
	}
	*len = data_len;
	return data;
}

void Request::initHeader() {
	header["Host"] = string(ip);
	header["Connection"] = "keep-alive";
	header["User-Agent"]=" Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/89.0.4389.90 Safari/537.36";
}

Request::Request(const char* url) {
	if( get_ip_port_path(url, &ip, &port, &path) ) {
		printf("wrong format url\n");
		exit(-1);
	}
	initHeader();
	sock = mySocket(ip, port); // 这里是construct or copy callback?
}

int Request::Connect() {
	sock = mySocket(ip, port);
	return 0;
}

void Request::Send() {
	// construct header and send
	ostringstream os;
	os<<"GET "<<path<<" HTTP/1.1\r\n";
	for(auto kv : header ) 
		os<<kv.first<<": "<<kv.second<<"\r\n";
	os<<"\r\n";
	string tmps = os.str();
	const char* ss = tmps.c_str();
#ifdef DEBUG
	printf("The message send!\n%s", ss);
#endif
	send(sock.sock, ss, strlen(ss), 0) ;
}

void* Request::RecvBlock() {
	int len;
	void* block = sock.readAsLongAsICan(&len);
	fwrite(block, len, 1, stdout);
	return block;
}

int Request::Recv() {
	char* responseLine = sock.readLine();
	int status=0;
	char dummy[512];
	sscanf(responseLine, "%s %d", dummy,&status);

	header["Cookie"] = "";
	while(1) {
		char* line = sock.readLine();
		if( strlen(line)==0 ) break;
		extract_key_value(line);
		free(line);
	} 

	if(header.count("Content-Length")) {
		int content_len = atoi(header["Content-Length"].c_str());
		char* html = (char*) malloc(content_len+10);
		html[content_len] = '\0';
		while(content_len>0) {
			int rz = recv(sock.sock, html, content_len, 0);
			content_len-=rz;
		}
		printf("\n\n\n Data Received!\n %s", html);
		free(html);
	} else {
		void* html = RecvBlock();
		free(html);
	}

/* TODO
	if(status==302) { // set cookie and send back
		printf("Send back again\n");
		Connect();
		Send();
		Recv();
	}
*/
	// read response body
	return 0;
}
