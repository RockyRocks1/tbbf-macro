#include <pixel/GdiPixelCapture.h>
#include <pixel/WGCPixelCapture.h>
#include <http/HttpClient.h>
#include <iostream>
#include <thread>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
/*
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
	*/
int main() {
	curl_global_init(CURL_GLOBAL_DEFAULT);
	{
		HttpClient client;
		Response res = client.get("https://httpbun.com/");
		std::cout << "Got curl code:  " << res.code << std::endl;
		std::cout << res.headers << std::endl;
		std::cout << res.body << std::endl;
	}
	curl_global_cleanup();
}