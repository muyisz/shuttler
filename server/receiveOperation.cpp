#include "receiveOperation.h"
#include <cstdio>
#include <cstring>

receiveOperation::receiveOperation() {
	dataSize = 200;
	data = new char[dataSize];
	dataUse = new char[dataSize];
	virtualKeyboard = new int[dataSize];
	memset(virtualKeyboard, 0, dataSize * sizeof(int));
}

receiveOperation::~receiveOperation() {
	delete[]data;
	delete[]dataUse;
	delete[]virtualKeyboard;
}

void receiveOperation::copyData(int k,int begin) {
	for (int i = begin; i < k; i++) {
		dataUse[i] = data[i];
	}
}

void receiveOperation::rebuffer(int k, int len) {
	for (int i = 0; i < len - k; i++) {
		data[i] = data[i + k];
	}
}

void receiveOperation::charToPoint(POINT& mouse) {
	mouse.x = 0;
	mouse.y = 0;
	int size = 9;
	int po = 1;
	while (size > 0) {
		mouse.x += (dataUse[size--] - '0') * po;
		po *= 10;
	}
	size = 19;
	po = 1;
	while (size > 10) {
		mouse.y += (dataUse[size--] - '0') * po;
		po *= 10;
	}
}

void receiveOperation::getData() {
	printf("start recv!");
	int readLen = 0;
	int size = 0;
	int flag = 0;
	while (1) {
		int begin = 0;
		if (flag) {
			copyData(flag, begin);
			begin += flag;
			flag = 0;
		}
		do
		{
			readLen = recv(recvSocket, data, dataSize, 0);
			if (size + readLen <= dataSize) {
				copyData(readLen, begin);
				size += readLen;
				begin += readLen;
				if (size == dataSize) {
					size = 0;
					flag = 0;
					break;
				}
			}
			else {
				int k = (dataSize - size);
				size += k;
				copyData(k, begin);
				size = readLen - k;
				rebuffer(k, readLen);
				flag = size;
				break;
			}

		} while (true);
		POINT mouse;
		charToPoint(mouse);
		SetCursorPos(mouse.x, mouse.y);
		performOperation();
	}
}

void receiveOperation::performOperation() {
	if (virtualKeyboard[0x1+20] == 1 && dataUse[0x1 + 20] == 0) {
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		virtualKeyboard[0x1 + 20] = 0;
	}
	else if (virtualKeyboard[0x1 + 20] == 0 && dataUse[0x1 + 20] == 1) {
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		virtualKeyboard[0x1 + 20] = 1;
	}
	if (virtualKeyboard[0x2+20] == 1 && dataUse[0x2 + 20] == 0) {
		mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
		virtualKeyboard[0x2 + 20] = 0;
	}
	else if (virtualKeyboard[0x2 + 20] == 0 && dataUse[0x2 + 20] == 1) {
		mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
		virtualKeyboard[0x2 + 20] = 1;
	}
	for (int i = 0x1 + 2; i <= 0x91; i++) {
		int k = i + 20;
		if (virtualKeyboard[k] == 1 && dataUse[k] == 0) {
			keybd_event(i, 0, KEYEVENTF_KEYUP, 0);
			virtualKeyboard[k] = 0;
		}
		else if (virtualKeyboard[k] == 0 && dataUse[k] == 1) {
			keybd_event(i, 0, 0, 0);
			virtualKeyboard[k] = 1;
		}
	}
}

void receiveOperation::setHEvent(HANDLE h) {
	this->hEvent = h;
}

void receiveOperation::getSocket() {
	
	WaitForSingleObject(hEvent, INFINITE);
	printf("开始执行接受程序\n");
	WORD sockVersion = MAKEWORD(1, 1);
	WSADATA data;
	if (WSAStartup(sockVersion, &data) != 0) {
		printf("套接字版本出错!\n");
		return;
	}
	recvSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (recvSocket == INVALID_SOCKET) {
		printf("invalid socket!");
		return;
	}
	sockaddr_in serAddr;
	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(22346);
	serAddr.sin_addr.S_un.S_addr = inet_addr(clientIP);
	if (connect(recvSocket, (sockaddr*)&serAddr, sizeof(serAddr)) == SOCKET_ERROR) {
		printf("连接错误\n");
		closesocket(recvSocket);
		return;
	}
	getData();
	closesocket(recvSocket);
	WSACleanup();
}

void receiveOperation::setClientIP(char IP[]) {
	clientIP = IP;
}