#pragma once
#include<Windows.h>

class screenShots
{
public:
	screenShots();
	~screenShots();
	void printBmp(char* name);
private:
	HDC hDC;
	HDC comHDC;
	int width;
	int height;
	HBITMAP hBitmap;
	BITMAPINFOHEADER bmiHeader;
	DWORD dwBmpSize;
	DWORD dwSizeofDIB;
	BITMAPFILEHEADER bmfHeader;
	char* lpbitmap;
};

