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
    SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
    HANDLE hMutex = CreateMutexA(NULL, TRUE, "ROBLOX_singletonEvent");
    winrt::init_apartment();

    curl_global_init(CURL_GLOBAL_DEFAULT);
    {
        HWND roblox = FindWindow(nullptr, "Roblox");
        MacroInstanceLaunchInfo launchInfoAlt = {};
        TbbfMacroManager manager;
        if (roblox)
            manager.LaunchMacroInstance(roblox, launchInfoAlt);
        else
            manager.LaunchMacroInstance(launchInfoAlt);
        manager.Start();

        while (true) {
            if (GetAsyncKeyState(VK_OEM_6) < 0)
            {
                if (manager.IsRunning())
                    manager.Stop();
            }
            Sleep(100);
        }
    }
    curl_global_cleanup();

    winrt::uninit_apartment();
    return 0;
}