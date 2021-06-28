#pragma once
#include <Windows.h>
class sendOperation
{
private:
	char* data;
	char* serverIP;
	int targetX;
	int targetY;
	SOCKET sendSocket;
	HANDLE hEvent;
public:
	sendOperation();
	~sendOperation();
	void setServerIP(char[]);
	void pointToChar(POINT&);
	void getOperation();
	void setHEvevt(HANDLE);
	void sendOper();
	void getSocket();
};

