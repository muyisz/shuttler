#pragma once
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996)

class receiveOperation {
private:
	int leftStatic;
	int rightStatic;
	int dataSize;
	int* virtualKeyboard;
	char* dataUse;
	char* clientIP;
	SOCKET recvSocket;
	HANDLE hEvent;
public:
	receiveOperation();
	~receiveOperation();
	void charToPoint(POINT&);
	void getData();
	void getSocket();
	void setClientIP(char[]);
	void setHEvent(HANDLE);
	void performOperation();
};

