#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include "request.h"
#include "filter.h"


int main() { 
	// prepare the header.txt firstly, the host in header.txt will be used as priority.
	char ipstr[] = "aqakgj.com";
	void *buffer = getResponse(ipstr , 80) ;
	//printf("Response %s\n", (char*) buffer);
	dealingWithResponse(buffer);
	printf("Done\n");
	return 0;
}
