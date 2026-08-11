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
        HANDLE hMutex = CreateMutexA(NULL, TRUE, "ROBLOX_singletonEvent");

        
        TbbfMacroManager manager;
        manager.LaunchMacroInstance(launchInfoAlt);
        Sleep(10000000);
    }
    curl_global_cleanup();

    winrt::uninit_apartment();
    return 0;
}