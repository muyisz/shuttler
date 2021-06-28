#pragma once
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996)

class screenShots;

class videoSending
{
private:
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
	int getFileSize(const char* strFileName);
	void sizeToChar(int size);
	void sendVideo();
	void listeningClient();
	void setClientIP(char[]);
	void setHEvent(HANDLE);
};

