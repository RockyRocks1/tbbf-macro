#include <pixel/GdiPixelCapture.h>
#include <pixel/WGCPixelCapture.h>
#include <http/HttpClient.h>
#include <iostream>
#include <thread>
#include <roblox/RobloxWeb.h>
#include <roblox/RobloxLauncher.h>
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
		//you gotta edit these values to get it working
		HANDLE hMutex = CreateMutexW(NULL, TRUE, L"ROBLOX_singletonEvent"); // testing multi acc
		RobloxWeb web("...");
		std::string authTicket = web.GetAuthenticationTicket().value_or("");
		uint64_t gameId = 1;
        std::string gamePath = "C:\\Users\\...\\AppData\\Local\\Roblox\\Versions\\...\\RobloxPlayerBeta.exe";
		RobloxLauncher launcher(gamePath);

		launcher.JoinPrivateGame(gameId, "...", authTicket);
	}
	curl_global_cleanup();
}