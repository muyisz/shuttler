#include "videoSending.h"
#include <fstream>
#include <iostream>
#include "screenShots.h"

videoSending::videoSending() {
	screenshots = new screenShots;
	bufferSize = 5000000;
	buffer = new char[bufferSize];
	sendSize = new char[10];
}

videoSending::~videoSending() {
	delete screenshots;
	delete[]buffer;
	delete[]sendSize;
}

int videoSending::getFileSize(const char* strFileName) {
	FILE* fp = fopen(strFileName, "r");
	fseek(fp, 0L, SEEK_END);
	long size = ftell(fp);
	fclose(fp);
	return size;
}

void videoSending::setClientIP(char IP[]) {
	clientIP = IP;
}

void videoSending::sizeToChar(int size) {
	for (int i = 9; i >= 0; i--) {
		sendSize[i] = size % 10 + '0';
		size /= 10;
	}
}

void videoSending::sendVideo() {
	printf("传输开始\n");
	int haveSend = 0;
	int readLen = 0;
	char srcFileName[10] = "6.png";
	SetEvent(hEvent);
	while (1) {
		haveSend = 0;
		screenshots->printBmp(srcFileName);
		std::ifstream srcFile;
		int size = getFileSize(srcFileName);
		sizeToChar(size);
		send(sendSocket, sendSize, 10, 0);
		srcFile.open(srcFileName, std::ios::binary);
		while (!srcFile.eof()) {
			srcFile.read(buffer, bufferSize);
			readLen = srcFile.gcount();
			send(sendSocket, buffer, readLen, 0);
			haveSend += readLen;
		}
	}
}

void videoSending::listeningClient() {
	WORD edition;
	edition = MAKEWORD(1, 1);
	WSADATA wsa_data;
	int err = WSAStartup(edition, &wsa_data);
	if (err != 0) {
		std::cout << "套接字版本绑定失败" << std::endl;
		return;
	}
	if (LOBYTE(wsa_data.wVersion) != 1 || HIBYTE(wsa_data.wVersion) != 1) {
		std::cout << "套接字版本出错" << std::endl;
		return;
	}
	SOCKET slisten = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (slisten == INVALID_SOCKET) {
		printf("套接字错误\n");
		return;
	}
	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(22345);
	sin.sin_addr.S_un.S_addr = INADDR_ANY;
	if (bind(slisten, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR) {
		printf("bind错误!\n");
	}
	if (listen(slisten, 5) == SOCKET_ERROR) {
		printf("监听错误!\n");
		return;
	}
	sockaddr_in remoteAddr;
	int nAddrlen = sizeof(remoteAddr);
	while (true) {
		printf("等待连接...\n");
		sendSocket = accept(slisten, (SOCKADDR*)&remoteAddr, &nAddrlen);
		if (sendSocket == INVALID_SOCKET) {
			printf("链接错误 !\n");
			continue;
		}
		printf("接受到连接!\n");
		sendVideo();
	}
}

void videoSending::setHEvent(HANDLE h) {
	this->hEvent = h;
}