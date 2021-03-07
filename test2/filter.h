#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

void work(char* s) {
	char*p = s;
	while(1) {
		p = strstr(p, "<img"); if(!p) return;
		p = strstr(p, "src"); if(!p) return;
		p = strtok(p, "="); 
		p = strtok(NULL, "\" >");
		if(!p) return;
		printf("%s\n",p);
		p+=strlen(p)+1;
	}
}

