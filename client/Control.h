#pragma once
#include "getVideo.h"
#include "sendOperation.h"

class Control
{
private:
	getVideo* ge;
	sendOperation* se;
	char serverIP[20];
	HANDLE hEvent;
public:
	Control();
	~Control();
	void getServerIP();
	void begin();
};

