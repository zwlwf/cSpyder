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
	getaddrinfo(ipstr, "80", &hints, &sock_addr);

	int flag = connect(sock, (struct sockaddr*)sock_addr->ai_addr, sizeof(struct sockaddr) );
	if(flag ){
		printf("connection failed \n");
		return -1;
	}
	return sock;
}

char* readHeader(const char* fname) {
	FILE* fp = fopen(fname, "r");
	fseek(fp, 0, SEEK_END);
	int n = ftell(fp) + 1000;
	fseek(fp, 0, SEEK_SET);
	char *data = malloc(n);
	char c;
	int cur =0;
	while( (c = fgetc(fp))!= EOF ) {
		if(c=='\n') {
			data[cur++] = '\r';
			data[cur++] = '\n';
		} else data[cur++] = c;
	}
	data[cur] = '\0';
	data = realloc(data, cur);
	fclose(fp);
	return data;
}

char* getResponse(char* ipstr, short port) { // the default port is 80 for http
	int csock = createSock_client(ipstr, port);
	char* req = readHeader("header.txt"); // store the header in header.txt, add an empty line in the end
	send(csock, req, strlen(req), 0);
	const int alloc_len = 100000;
	int data_len = 0;
	char* rdata = malloc(alloc_len);
	int tot = 0;
	char* cur = rdata;
	while(1) {
		cur = rdata+data_len;
		int rsize = recv(csock, cur, alloc_len-data_len, 0);
		printf("%d data received!\n", rsize);
		if(rsize == 0) break;
		data_len += rsize;
	}
	rdata[data_len] = '\0';
	printf("%s\n", rdata);

	free(req);
	close(csock);
	return rdata;
}

