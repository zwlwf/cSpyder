// get response 
char* getResponse(char* ipstr, short port);

// store your header in header.txt, then read with following function
// this request header can be copied from CTRL+SHIFT+I in chrome
char* readHeader(const char* fname);

// get socket for header
int createSock_client( const char* ipstr, short port);
