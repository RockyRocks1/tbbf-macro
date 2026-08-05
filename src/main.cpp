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

        std::cout << "Waiting..." << std::endl;
        auto startTime = std::chrono::high_resolution_clock::now();
        while (true) {
            if (!game)
                break;
            
            const FrameView currentFrame = game->GetLatestFrame();
            if (currentFrame.data) {
                if (game->WasGameLoaded(currentFrame)) {
                    std::cout << "Game loaded" << std::endl;
                    break;
                }
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        std::cout << "Time: " <<  std::fixed << duration << std::endl;
    }
    curl_global_cleanup();

    winrt::uninit_apartment();
    return 0;
}