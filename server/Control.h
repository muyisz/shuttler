#pragma once
#include "receiveOperation.h"
#include "screenShots.h"
#include "videoSending.h"

class Control {
private:
	receiveOperation* re;
	screenShots* sc;
	videoSending* vi;
	char clientIP[20];
	HANDLE hEvent;
public:
	Control();
	~Control();
	void getIP();
	void begin();
};

