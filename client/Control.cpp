#include "Control.h"
#include <iostream>
#include <thread>

Control::Control() {
	ge = new getVideo;
	se = new sendOperation;
	getServerIP();
	ge->setServerIP(serverIP);
	se->setServerIP(serverIP);
	hEvent = CreateEvent(NULL, FALSE, FALSE, "event");
	ge->SetHEvent(hEvent);
	se->setHEvevt(hEvent);
}

Control::~Control() {
	delete ge;
	delete se;
}

void Control::getServerIP() {
	printf("ÇëÊäÈë·þÎñÆ÷IP\n");
	std::cin >> serverIP;
}

void Control::begin() {
	std::thread getVi(&getVideo::getSocket, *ge);
	std::thread sendOp(&sendOperation::getSocket, *se);
	getVi.join();
	sendOp.join();
}