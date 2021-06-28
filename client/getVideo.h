#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <winsock2.h>
#include <fstream>
#pragma comment(lib,"ws2_32.lib")

class getVideo
{
private:
	char* serverIP;
	SOCKET recvSocket;
	int bufferSize;
	char* buffer;
	char* buffer_;
	HANDLE hEvent;
public:
	getVideo();
	~getVideo();
	void bufferMerge(int,int);
	void SetHEvent(HANDLE);
	void recvVideo();
	void getSocket();
	void setServerIP(char[]);
	void rebuffer(int, int);
	int charToInt();
};

