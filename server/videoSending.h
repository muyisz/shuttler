#pragma once
#include <winsock2.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996)

class screenShots;

class videoSending
{
private:
	int wigth;
	int high;
	screenShots* screenshots;
	char *clientIP;
	SOCKET sendSocket;
	int bufferSize;
	char *buffer;
	char *sendSize;
	HANDLE hEvent;
public:
	videoSending();
	~videoSending();
	int makeCode(cv::Mat,cv::Mat);
	int getFileSize(const char*);
	void sizeToChar(int, int);
	void setBuffer(cv::Mat, cv::Mat, int, int, int);
	void sendVideo();
	void listeningClient();
	void setClientIP(char[]);
	void setHEvent(HANDLE);
};

