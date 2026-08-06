#include <iostream>
#include <Windows.h>
#include <thread>
#include <chrono>

#include <winrt/Windows.Foundation.h> 
#include <curl/curl.h>

#include <roblox/RobloxGame.h>
#include <tbbf/TbbfMacroManager.h>
#include <macro/MacroInstanceLaunchInfo.h>
int main() {
    winrt::init_apartment();

    curl_global_init(CURL_GLOBAL_DEFAULT);
    {
        MacroInstanceLaunchInfo launchInfo = {
            .robloSecurity = "...",
            .gameId = 5977347869,
            .privateServerAccessCode = "...",
            .gamePath = "C:\\Users\\...\\AppData\\Local\\Roblox\\Versions\\version-d584fb6c717a43d9\\RobloxPlayerBeta.exe",
        };
        TbbfMacroManager manager;
        manager.LaunchMacroInstance(launchInfo);
        Sleep(100000);
    }
    curl_global_cleanup();

    winrt::uninit_apartment();
    return 0;
}