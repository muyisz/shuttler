#pragma once
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996)

class receiveOperation {
private:
	int leftStatic;
	int rightStatic;
	int dataSize;
	char* data;
	char* dataUse;
	char* clientIP;
	SOCKET recvSocket;
	HANDLE hEvent;
public:
	receiveOperation();
	~receiveOperation();
	void copyData(int, int);
	void rebuffer(int, int);
	void charToPoint(POINT&);
	void getData();
	void getSocket();
	void setClientIP(char[]);
	void setHEvent(HANDLE);
};
