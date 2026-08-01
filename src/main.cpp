#include <pixel/GdiPixelCapture.h>
#include <iostream>
int main() {
	HWND hwnd = FindWindowW(NULL, L"Roblox");
	POINT mousePos;
	GetCursorPos(&mousePos);
	std::cout << "Mouse X: " << mousePos.x << " | Mouse Y: " << mousePos.y << std::endl;
	
	GdiPixelCapture pixelReader;
	std::cout << pixelReader.Initialize(NULL) << std::endl;
	std::cout << pixelReader.CaptureRegion(0, 0, 1900, 1000) << std::endl;
	ColorRGBA color = pixelReader.GetPixel(mousePos.x, mousePos.y);
	std::cout << (int)color.r << " " << (int)color.g << " " << (int)color.b << " " << (int)color.a << std::endl;
	return 0;
}