#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include "request.h"
#include "filter.h"

char *fmt = "GET %s HTTP/1.1\r\n"
"Host: %s\r\n"
"Connection: keep-alive\r\n"
"Accept: text/html, */*; q=0.01\r\n"
"User-Agent: Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/89.0.4389.90 Safari/537.36\r\n"
"Accept-Encoding: gzip, deflate, br\r\n"
"Accept-Language: zh-CN,zh;q=0.9,en;q=0.8\r\n\r\n";

void usage() {
	printf("./a.out url_of_source\n");
}

int main(int argc, char** argv) { 
	// prepare the header.txt firstly, the host in header.txt will be used as priority.
	char ipstr[1024] = {0};
	if( argc==1 ) {
		usage();
		return 0;
	}
	char* s = argv[1], *p;
	short port = 80;
	int i=0;
	if( p = strstr(s, "://") ) p+=3;
	else return 0;
	s = p;
	while(s[i] && s[i]!='/' && s[i]!=':') i++;
	strncpy(ipstr, s, i);
	s+=i;
	if(*s==':') {
		s++;
		port = strtol(s, &p, 10);
	} else 
		p = s;
	
	char header[1024<<4];
	sprintf(header,fmt, p, ipstr);
	void *buffer = getResponse(ipstr , port, header) ;
	printf("Response %s\n", (char*) buffer);
	//dealingWithResponse(buffer);
	printf("Done\n");
	return 0;
}
