#include "getVideo.h"
#pragma warning(disable:4996)

getVideo::getVideo() {
	bufferSize = 50000000;
	buffer = new char[bufferSize];
	wigth = 1280;
	high = 720;
	blockSize = 4;
}
//120
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
	printf("%d\n", fileSize);
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
		for (int i = 0; i < fileSize;) {//横 右下 左下 竖
			int x = buffer[i++] * 127;
			x += buffer[i++];
			int y = buffer[i++] * 127;
			y += buffer[i++];
			int type = buffer[i++];
			if (type == 1) {
				for (int insideY = 0; insideY < blockSize * 3; insideY += 3) {//横
					for (int insideX = 0; insideX < blockSize; insideX++) {
						for (int j = 0; j < 3; j++) {
							img.data[(((y + insideY + j) * wigth) + (x + insideX))] = buffer[i + insideX * 3 + j];
						}
					}
				}
				i += blockSize * 3;
			}
			else if (type == 2) {
				for (int insideY = 0; insideY < blockSize * 3; insideY += 3) {//右下
					for (int insideX = 0; insideX < blockSize; insideX++) {
						int com = insideY / 3 + insideX;
						for (int j = 0; j < 3; j++) {
							img.data[(((y + insideY + j) * wigth) + (x + insideX))] = buffer[i + com + j];
						}
					}
				}
				i += (blockSize * 2 - 1) * 3;
			}
			else if (type == 3) {
				for (int insideY = 0; insideY < blockSize * 3; insideY += 3) {//左下
					for (int insideX = 0; insideX < blockSize; insideX++) {
						int com = insideX - insideY / 3;
						com += blockSize;
						for (int j = 0; j < 3; j++) {
							img.data[(((y + insideY + j) * wigth) + (x + insideX))] = buffer[i + com + j];
						}
					}
				}
				i += (blockSize * 2 - 1) * 3;
			}
			else {
				for (int insideY = 0; insideY < blockSize * 3; insideY += 3) {//竖
					for (int insideX = 0; insideX < blockSize; insideX++) {
						for (int j = 0; j < 3; j++) {
							img.data[(((y + insideY + j) * wigth) + (x + insideX))] = buffer[i + (insideY / 3) + j];
						}
					}
				}
				i += blockSize * 3;
			}
		}
		imshow("控制", img);
		cv::waitKey(1);
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