#include "mySocket.h"

int main(int argc, char**argv) {
	Request req(argv[1]);
	req.Send();
	//void* response = req.RecvBlock();
	req.Recv();
}
