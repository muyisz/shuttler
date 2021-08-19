#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <winsock2.h>
#include <fstream>
#pragma comment(lib,"ws2_32.lib")

class getVideo
{
private:
	int blockSize;
	char* serverIP;
	SOCKET recvSocket;
	int bufferSize;
	char* buffer;
	HANDLE hEvent;
	int wigth;
	int high;
public:
	getVideo();
	~getVideo();
	void SetHEvent(HANDLE);
	void recvVideo();
	void getSocket();
	void setServerIP(char[]);
	int charToInt(int);
};