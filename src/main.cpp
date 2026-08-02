#include <pixel/GdiPixelCapture.h>
#include <pixel/WGCPixelCapture.h>
#include <iostream>
#include <thread>
int main() {
	HWND hwnd = FindWindowW(NULL, L"Roblox");
	POINT mousePos;
	GetCursorPos(&mousePos);
	ScreenToClient(hwnd, &mousePos);
	std::cout << "Mouse X: " << mousePos.x << " | Mouse Y: " << mousePos.y << std::endl;
	
	WGCPixelCapture pixelReader;
	std::cout << pixelReader.Initialize(hwnd) << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	std::cout << pixelReader.CaptureRegion(0, 0, 1900, 1000) << std::endl;
	ColorRGBA color = pixelReader.GetPixel(mousePos.x, mousePos.y);
	std::cout << (int)color.r << " " << (int)color.g << " " << (int)color.b << " " << (int)color.a << std::endl;
	return 0;
}