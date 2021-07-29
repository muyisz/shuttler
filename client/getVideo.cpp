#include "getVideo.h"
#pragma warning(disable:4996)

getVideo::getVideo() {
	bufferSize = 50000000;
	buffer = new char[bufferSize];
	wigth = 1920;
	high = 1080;
}

getVideo::~getVideo() {
	delete[]buffer;
}

void getVideo::SetHEvent(HANDLE h) {
	hEvent = h;
}

int getVideo::charToInt(int pos) {
	int k = 0;
	int b = 1000000000;
	for (int i = 0; i < 10; i++) {
		k += (buffer[i + pos] - '0') * b;
		b /= 10;
	}
	return k;
}

void getVideo::recvVideo() {
	std::cout << "start recv!" << std::endl;
	cv::namedWindow("控制", cv::WINDOW_NORMAL);
	int readLen = 0;
	int img_size;
	char desFileName[10] = "1.png";
	int size = 0;
	int flag = 0;
	SetEvent(hEvent);

	std::ofstream desFile;
	desFile.open(desFileName, std::ios::binary);
	int getSize = 10;
	int pos = 0;
	while (pos < getSize) {
		int len0 = recv(recvSocket, buffer + pos, getSize - pos, 0);
		pos += len0;
	}
	pos = 0;
	int fileSize = charToInt(0);
	while (pos < fileSize) {
		int len0 = recv(recvSocket, buffer + pos, fileSize - pos, 0);
		pos += len0;
	}
	desFile.write(buffer, fileSize);
	cv::Mat img;
	desFile.close();
	img = cv::imread(desFileName, 1);
	imshow("控制", img);
	cv::waitKey(10);
	while (1) {
		int pos = 0;
		while (pos < getSize) {
			int len0 = recv(recvSocket, buffer + pos, getSize - pos, 0);
			pos += len0;
		}
		pos = 0;
		int fileSize = charToInt(0);
		fileSize -= 10;
		while (pos < fileSize) {
			int len0 = recv(recvSocket, buffer + pos, fileSize - pos, 0);
			pos += len0;
		}
		for (int i = 0; i < fileSize; i += 74) {
			int pos = charToInt(i);
			int x = pos / 10000;
			int y = pos % 10000;
			int size = i + 10;
			for (int insideX = 0; insideX < 8; insideX++) {
				for (int insideY = 0; insideY < 8; insideY++) {	
					int pos = (((y + insideY) * wigth) + (x + insideX));
					img.data[pos] = buffer[size++];
				}
			}
		}
		imshow("控制", img);
		cv::waitKey(10);
	}
}

void getVideo::getSocket() {
	WORD sockVersion = MAKEWORD(1, 1);
	WSADATA data;
	if (WSAStartup(sockVersion, &data) != 0)
	{
		printf("套接字版本出错!\n");
		return;
	}
	recvSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (recvSocket == INVALID_SOCKET)
	{
		printf("invalid socket!");
		return;
	}
	sockaddr_in serAddr;
	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(22345);
	serAddr.sin_addr.S_un.S_addr = inet_addr(serverIP);
	if (connect(recvSocket, (sockaddr*)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
	{
		printf("连接错误\n");
		closesocket(recvSocket);
		return;
	}
	recvVideo();
	closesocket(recvSocket);
	WSACleanup();
}

void getVideo::setServerIP(char IP[]) {
	serverIP = IP;
}