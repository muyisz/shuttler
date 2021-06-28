#include "screenShots.h"
#include <fstream>

void screenShots::printBmp(char* name) {
	if (BitBlt(comHDC, 0, 0, width, height, hDC, 0, 0, SRCCOPY))
	{
		ZeroMemory(lpbitmap, dwBmpSize);
		GetDIBits(comHDC, hBitmap, 0, (UINT)height, lpbitmap, (BITMAPINFO*)&bmiHeader, DIB_RGB_COLORS);
		std::ofstream of(name, std::ios_base::binary);
		of.write((char*)&bmfHeader, sizeof(BITMAPFILEHEADER));
		of.write((char*)&bmiHeader, sizeof(BITMAPINFOHEADER));
		of.write(lpbitmap, dwBmpSize);
		of.close();
	}
}

screenShots::screenShots() : lpbitmap(0) {
	hDC = GetDC(NULL);
	comHDC = CreateCompatibleDC(hDC);
	width = GetSystemMetrics(SM_CXSCREEN);
	height = GetSystemMetrics(SM_CYSCREEN);
	hBitmap = CreateCompatibleBitmap(hDC, width, height);
	SelectObject(comHDC, hBitmap);
	bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmiHeader.biWidth = width;
	bmiHeader.biHeight = height;
	bmiHeader.biPlanes = 1;
	bmiHeader.biBitCount = 16;
	bmiHeader.biCompression = BI_RGB;
	bmiHeader.biSizeImage = 0;
	bmiHeader.biXPelsPerMeter = 0;
	bmiHeader.biYPelsPerMeter = 0;
	bmiHeader.biClrUsed = 0;
	bmiHeader.biClrImportant = 0;
	dwBmpSize = (width * bmiHeader.biBitCount + 31) / 32 * 4 * height;
	dwSizeofDIB = dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);
	bmfHeader.bfSize = dwSizeofDIB;
	bmfHeader.bfType = 0x4D42;
	lpbitmap = new char[dwBmpSize];
}

screenShots::~screenShots() {
	DeleteDC(hDC);
	DeleteDC(comHDC);
	DeleteObject(hBitmap);
	delete lpbitmap;
}