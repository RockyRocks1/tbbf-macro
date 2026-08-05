#include <iostream>
#include <Windows.h>
#include <thread>
#include <chrono>

#include <winrt/Windows.Foundation.h> 
#include <curl/curl.h>

#include <roblox/RobloxGame.h>

int main() {
    winrt::init_apartment();

    curl_global_init(CURL_GLOBAL_DEFAULT);
    {
        HWND robloxHwnd = FindWindowA(nullptr, "Roblox");
        std::optional<RobloxGame> game = RobloxGame::FromHwnd(robloxHwnd);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        if (game) {
            game->UpdateClientBounds();
            IPixelCapture* pixelCapture = game->GetPixelCapture();
            std::cout << pixelCapture->CaptureClientRegion(game->GetClientBounds()) << std::endl;
            
            if (game->WasDisconnected())
                std::cout << "Disconnected Check Success..." << std::endl;
        }
    }
    curl_global_cleanup();

    winrt::uninit_apartment();
    return 0;
}