#include <unordered_map>

class mySocket {
public:
	int sock;
	mySocket(const char* ipstr, const char* portstr);
	mySocket(const mySocket& b) { 
		this->sock = b.sock;
	}
	mySocket(){}
	void operator=(const mySocket& b) { 
		this->sock = b.sock;
	}
	void* readAsLongAsICan(int* len);
	char* readLine();
};

class Request {
	mySocket sock;
	char* url;
	char* ip;
	char* port;
	char* path;
	std::unordered_map<std::string, std::string> header;
	void initHeader();
public:
	Request(const char* url);
	void Send();
	void* RecvBlock(); // receive all content
	int Recv();
};
