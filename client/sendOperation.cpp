#include "sendOperation.h"
#include <iostream>
#define KEY_DOWN(VK_NONAME) ((GetAsyncKeyState(VK_NONAME) & 0x8000) ? 1:0)

sendOperation::sendOperation() {
	data = new char[50];
	targetX = 1920;
	targetY = 1080;
}

sendOperation::~sendOperation() {
	delete[]data;
}

void sendOperation::setServerIP(char IP[]) {
	serverIP = IP;
}

void sendOperation::setHEvevt(HANDLE h) {
	hEvent = h;
}

void sendOperation::pointToChar(POINT& mouse) {
	int x = mouse.x;
	int y = mouse.y;
	int size = 9;
	while (x != 0) {
		int k = x % 10;
		data[size--] = k + '0';
		x /= 10;
	}
	size = 19;
	while (y != 0) {
		int k = y % 10;
		data[size--] = k + '0';
		y /= 10;
	}
}

void sendOperation::getOperation() {
	if (KEY_DOWN(MOUSE_MOVED)) {
		data[0] = 1;
	}
	else {
		data[0] = 0;
	}
	if (KEY_DOWN(MOUSE_EVENT)) {
		data[10] = 1;
	}
	else {
		data[10] = 0;
	}
}

void sendOperation::sendOper() {
	printf("传输开始\n");
	int haveSend = 0;
	int readLen = 0;
	int window_sizeX;
	int window_sizeY;
	POINT mouse;
	WaitForSingleObject(hEvent, INFINITE);
	HWND Control_window = FindWindow(NULL, "控制");
	if (Control_window != NULL) {
		std::cout << "窗口获取成功!" << std::endl;
	}
	else {
		std::cout << "窗口获取失败" << std::endl;
		return;
	}
	RECT size;
	while (1) {
		memset(data, '0', 50);
		GetCursorPos(&mouse);
		GetWindowRect(Control_window, &size);
		size.top += 20;
		size.bottom -= 20;
		size.right -= 20;
		window_sizeX = size.right - size.left;
		window_sizeY = size.bottom - size.top;
		ScreenToClient(Control_window, &mouse);
		if (mouse.x >= 0 && mouse.y >= 0 && mouse.x <= window_sizeX && mouse.y <= window_sizeY) {
			mouse.x = (int)(((double)mouse.x) * targetX / (double)window_sizeX);
			mouse.y = (int)(((double)mouse.y) * targetY / (double)window_sizeY);
			getOperation();
			pointToChar(mouse);
			send(sendSocket, data, 50, 0);
		}
		Sleep(10);
	}
}

void sendOperation::getSocket() {
	WORD edition;
	edition = MAKEWORD(1, 1);
	WSADATA wsa_data;
	int err = WSAStartup(edition, &wsa_data);
	if (err != 0) {
		std::cout << "套接字版本绑定失败" << std::endl;
		return;
	}
	if (LOBYTE(wsa_data.wVersion) != 1 || HIBYTE(wsa_data.wVersion) != 1) {
		std::cout << "套接字版本出错" << std::endl;
		return;
	}
	SOCKET slisten = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (slisten == INVALID_SOCKET)
	{
		printf("套接字错误\n");
		return;
	}
	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(22346);
	sin.sin_addr.S_un.S_addr = INADDR_ANY;
	if (bind(slisten, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)
	{
		printf("bind错误!\n");
	}
	if (listen(slisten, 5) == SOCKET_ERROR)
	{
		printf("监听错误!\n");
		return;
	}
	sockaddr_in remoteAddr;
	int nAddrlen = sizeof(remoteAddr);
	while (true) {
		printf("等待连接...\n");
		sendSocket = accept(slisten, (SOCKADDR*)&remoteAddr, &nAddrlen);
		if (sendSocket == INVALID_SOCKET) {
			printf("链接错误 !\n");
			continue;
		}
		printf("接受到连接!\n");
		sendOper();
	}
}

