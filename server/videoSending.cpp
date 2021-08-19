#include "videoSending.h"
#include <fstream>
#include <iostream>
#include <cmath>
#include "screenShots.h"

videoSending::videoSending() {
	wigth = 1920;
	high = 1080;
	blockSize = 4;
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

int videoSending::setBuffer(cv::Mat last, cv::Mat next, int size, int x, int y) {//横 右下 左下 竖
	int sum = size;
	buffer[sum] = x / 127;
	buffer[sum + 1] = x % 127;
	buffer[sum + 2] = y / 127;
	buffer[sum + 3] = y % 127;
	sum += 4;
	int now = 0;
	for (int insideY = 0; insideY < blockSize * 3; insideY += 3) {//横
		for (int insideX = 0; insideX < blockSize; insideX++) {
			for (int i = 0; i < 3; i++) {
				int pos = next.data[(((y + insideY + i) * wigth) + (x + insideX))];
				int buf = next.data[(((y + i) * wigth) + (x + insideX))];
				now += abs(pos - buf);
			}
		}
	}
	int mindif = now;
	int type = 1;
	now = 0;
	for (int insideY = 0; insideY < blockSize * 3; insideY += 3) {//右下
		for (int insideX = 0; insideX < blockSize; insideX++) {
			int com = insideY / 3 + insideX;
			int ix = 0;
			int iy = 0;
			if (com % 2 == 0) {
				ix = com / 2;
				iy = com / 2 * 3;
			}
			else {
				ix = com / 2;
				iy = (ix + 1) * 3;
			}
			for (int i = 0; i < 3; i++) {
				int pos = next.data[(((y + insideY + i) * wigth) + (x + insideX))];
				int buf = next.data[(((y + iy + i) * wigth) + (x + ix))];
				now += abs(pos - buf);
			}
		}
	}
	if (now < mindif) {
		mindif = now;
		type = 2;
	}
	now = 0;
	for (int insideY = 0; insideY < blockSize * 3; insideY += 3) {//左下
		for (int insideX = 0; insideX < blockSize; insideX++) {
			int com = insideX - insideY / 3;
			int ix = 0;
			int iy = 0;
			if (com >= 0) {
				ix = com;
				iy = 0;
			}
			else {
				ix = 0;
				iy = (-com) * 3;
			}
			for (int i = 0; i < 3; i++) {
				int pos = next.data[(((y + insideY + i) * wigth) + (x + insideX))];
				int buf = next.data[(((y + iy + i) * wigth) + (x + ix))];
				now += abs(pos - buf);
			}
		}
	}
	if (now < mindif) {
		mindif = now;
		type = 3;
	}
	now = 0;
	for (int insideY = 0; insideY < blockSize * 3; insideY += 3) {//竖
		for (int insideX = 0; insideX < blockSize; insideX++) {
			for (int i = 0; i < 3; i++) {
				int pos = next.data[(((y + insideY + i) * wigth) + (x + insideX))];
				int buf = next.data[(((y + insideY + i) * wigth) + (x))];
				now += abs(pos - buf);
			}
		}
	}
	if (now < mindif) {
		mindif = now;
		type = 4;
	}
	buffer[sum++] = type;
	if (type == 1) {//横
		for (int insideX = 0; insideX < blockSize; insideX++) {
			for (int i = 0; i < 3; i++) {
				buffer[sum++] = next.data[(((y + i) * wigth) + (x + insideX))];
			}
		}
	}
	else if (type == 2) {//右下
		int k = blockSize * 2 - 1;
		for (int com = 0; com < k; com++) {
			int ix = 0;
			int iy = 0;
			if (com % 2 == 0) {
				ix = com / 2;
				iy = com / 2 * 3;
			}
			else {
				ix = com / 2;
				iy = (ix + 1) * 3;
			}
			for (int i = 0; i < 3; i++) {
				buffer[sum++] = next.data[(((y + iy + i) * wigth) + (x + ix))];
			}
		}
	}
	else if (type == 3) {//左下
		int k = blockSize * 2 - 1;
		for (int com = 0; com < k; com++) {
			int ix = 0;
			int iy = 0;
			if (com - blockSize + 1 >= 0) {
				ix = com;
				iy = 0;
			}
			else {
				ix = 0;
				iy = (-(com - blockSize + 1)) * 3;
			}
		//	printf("%d %d %d %d\n", ix, iy, x, y);
			for (int i = 0; i < 3; i++) {
				buffer[sum++] = next.data[(((y + iy + i) * wigth) + (x + ix))];
			}
		}
	}
	else {//竖
		for (int insideY = 0; insideY < blockSize * 3; insideY += 3) {
			for (int i = 0; i < 3; i++) {
				buffer[sum++]= next.data[(((y + insideY + i) * wigth) + (x))];
			}
		}
	}
	return sum - size;
}
int videoSending::makeCode(cv::Mat last, cv::Mat next) {
	int size = 10;
	unsigned char* lastData = last.data;
	unsigned char* nextData = next.data;
	for (int x = 0; x < wigth; x += blockSize) {
		for (int y = 0; y < high * 3; y += blockSize * 3) {
			int flag = 0;
			for (int insideX = 0; insideX < blockSize; insideX++) {
				for (int insideY = 0; insideY < blockSize * 3; insideY++) {
					int pos = (((y + insideY) * wigth) + (x + insideX));
					if (lastData[pos] != nextData[pos]) {
						size += setBuffer(last, next, size, x, y);
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

void videoSending::sizeToChar(int size, int pos) {
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
	while (true) {
		last = cv::imread(srcFileName, 1);
		screenshots->printBmp(srcFileName);
		next = cv::imread(srcFileName, 1);
		size = makeCode(last, next);
	//	printf("%d\n", size);
		sizeToChar(size, 0);
		send(sendSocket, buffer, size, 0);
		Sleep(50);
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