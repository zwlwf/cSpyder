#define _POSIX_C_SOURCE 200112L
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <ctype.h>

int createSock_client( const char* ipstr, short port) {
	int sock = socket( AF_INET, SOCK_STREAM, 0);
	struct addrinfo hints, *sock_addr;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	//hints.sin_addr.s_addr = inet_addr(ipstr);
	char portname[64];
	sprintf(portname, "%d", port);
	getaddrinfo(ipstr, portname, &hints, &sock_addr);

	int flag = connect(sock, (struct sockaddr*)sock_addr->ai_addr, sizeof(struct sockaddr) );
	if(flag ){
		printf("connection failed \n");
		return -1;
	}
	return sock;
}

char* readHeader(const char* fname) {
	FILE* fp = fopen(fname, "r");
	int flag = 1; // dos[0] or unix[1] style
	fseek(fp, 0, SEEK_END);
	int n = ftell(fp) + 1000;
	fseek(fp, 0, SEEK_SET);
	char *data = malloc(n);
	char c;
	int cur =0;
	while( (c = fgetc(fp))!= EOF ) {
		if(c=='\r') {
			flag = 0;
			break;
		}
		if(c=='\n') {
			data[cur++] = '\r';
			data[cur++] = '\n';
		} else data[cur++] = c;
	}
	if( flag==0 ) {
		fseek(fp, 0, SEEK_SET);
		cur = fread(data, n, 1, fp);
	}
	data[cur] = '\0';
	while( strcmp(&data[cur-4], "\r\n\r\n")!=0 ) {
		// request must end with one empty line
		data[cur++] = '\r';
		data[cur++] = '\n';
		data[cur]='\0';
	}

	data = realloc(data, cur);
	fclose(fp);
	return data;
}

char* getResponse(char* ipstr, short port, char* req) { // the default port is 80 for http
	if( !req ) {
		// req not give, read frem header.txt
		req = readHeader("header.txt"); // store the header in header.txt, add an empty line in the end
		do {
			// try to get host and port
			char* p= strstr(req, "Host:");
			if(!p) break;
			p+= strlen("Host:");
			int i = 0;
			while(p[i] && p[i]==' ') i++;
			int j = i;
			while(p[j] && p[j]!=':' && p[j]!=' ' && p[j]!='\r' ) j++;
			ipstr = calloc(j-i+1,1);
			strncpy(ipstr, &p[i], j-i);
			if(p[j] && p[j]==':') {
				port=0;
				j++;
				while(p[j] && p[j]>='0' && p[j]<='9') {
					port = port*10+(p[j]^48);
					j++; 
				}
			}
			printf("host:%s and port:%d in header file used!\n", ipstr, port);
			
		} while(0);
	}
	
	int csock = createSock_client(ipstr, port);
	send(csock, req, strlen(req), 0);
	const int alloc_len = 100000;
	int data_len = 0;
	char* rdata = malloc(alloc_len);
	int tot = 0;
	char* cur = rdata;
	time_t pre, now;
	pre = time(NULL);
	double LongestTimeWait = 1.0; // the time client will wait for message
	cur = rdata+data_len;
	int rsize = recv(csock, cur, alloc_len-data_len, 0); // the first data must be blocked
	while(1) {
		if(rsize>0) {
			pre = time(NULL);
			printf("%d data received!\n", rsize);
			data_len += rsize;
		} else {
			now = time(NULL);
			if(now-pre>LongestTimeWait)
				break;
		}

		cur = rdata+data_len;
		rsize = recv(csock, cur, alloc_len-data_len, MSG_DONTWAIT);
	}
	rdata[data_len] = '\0';
	//printf("%s\n", rdata);

	//free(req);
	close(csock);
	return rdata;
}

