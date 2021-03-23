# cSpyder
A simple web spyder by pure c language, for learning

A main function is:
`char* getResponse(char* ipstr, short port, char* request);`
Call function to get the response, than in main you can write 'filter.h' for dealing 
with the response.

The `request` is information that send to server. It can be constructed by input url
or read from file `header.txt`. Content in `header.txt` can be obtained in chrome(CTRL+SHIFT+I).
