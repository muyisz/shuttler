#include "Control.h"
#include <iostream>
#include <thread>

Control::Control() {
	re = new receiveOperation;
	sc = new screenShots;
	vi = new videoSending;
	getIP();
	vi->setClientIP(clientIP);
	re->setClientIP(clientIP);
	hEvent = CreateEvent(NULL, FALSE, FALSE, "event");
	vi->setHEvent(hEvent);
	re->setHEvent(hEvent);
}

Control::~Control() {
	delete re;
	delete sc;
	delete vi;
}

void Control::getIP() {
	printf("ÇëÊäÈë¿Í»§¶ËIP\n");
	std::cin >> clientIP;
}

void Control::begin() {
	std::thread sendVi(&videoSending::listeningClient, *vi);
	std::thread getDa(&receiveOperation::getSocket, *re);
	sendVi.join();
	getDa.join();
}