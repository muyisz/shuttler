#include "videoSending.h"
#include <fstream>
#include <iostream>
#include "screenShots.h"

videoSending::videoSending() {
	wigth = 1920;
	high = 1080;
	screenshots = new screenShots;
	bufferSize = 50000000;
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

void videoSending::setBuffer(cv::Mat last, cv::Mat next, int size, int x, int y) {//74位一个块
	int sum = size;
	sizeToChar(x * 10000 + y, size);
	size += 10;
	for (int insideX = 0; insideX < 8; insideX++) {
		for (int insideY = 0; insideY < 8; insideY++) {
			int pos = (((y + insideY) * wigth) + (x + insideX));
			buffer[size++] = next.data[pos];
		}
	}
}
int videoSending::makeCode(cv::Mat last, cv::Mat next) {
	int size = 10;
	unsigned char* lastData = last.data;
	unsigned char* nextData = next.data;
	for (int x = 0; x < wigth; x += 8) {
		for (int y = 0; y < high * 3; y += 8) {
			for (int insideX = 0; insideX < 8; insideX++) {
				int flag = 0;
				for (int insideY = 0; insideY < 8; insideY++) {
					int pos = (((y + insideY) * wigth) + (x + insideX));
					if (lastData[pos] != nextData[pos]) {
						setBuffer(last, next, size, x, y);
						size += 74;
						flag = 1;
						break;
					}
				}
				if (flag = 1)
					break;
			}
		}
	}
	return size;
}

void videoSending::setClientIP(char IP[]) {
	clientIP = IP;
}

void videoSending::sizeToChar(int size, int pos) {//10 
	for (int i = 9; i >= 0; i--) {
		buffer[i + pos] = size % 10 + '0';
		size /= 10;
	}
}

void videoSending::sendVideo() {
	printf("传输开始\n");
	int haveSend = 0;
	int readLen = 0;
	char srcFileName[10] = "6.png";
	SetEvent(hEvent);
	haveSend = 0;
	screenshots->printBmp(srcFileName);
	std::ifstream srcFile;
	int size = getFileSize(srcFileName);
	sizeToChar(size, 0);
	send(sendSocket, buffer, 10, 0);
	srcFile.open(srcFileName, std::ios::binary);
	while (!srcFile.eof()) {
		srcFile.read(buffer, bufferSize);
		readLen = srcFile.gcount();
		send(sendSocket, buffer, readLen, 0);
		haveSend += readLen;
	}
	cv::Mat last;
	cv::Mat next;
	while (1) {
		last = cv::imread(srcFileName, 1);
		screenshots->printBmp(srcFileName);
		next = cv::imread(srcFileName, 1);
		size = makeCode(last, next);
		sizeToChar(size, 0);
		printf("%d\n", size);
		send(sendSocket, buffer, size, 0);
		Sleep(20);
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