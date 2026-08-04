#include <iostream>
#include <Windows.h>
#include <thread>
#include <chrono>

#include <winrt/Windows.Foundation.h> 
#include <curl/curl.h>

int main() {
    winrt::init_apartment();

    curl_global_init(CURL_GLOBAL_DEFAULT);
    {
        // ...
    }
    curl_global_cleanup();

    winrt::uninit_apartment();
    return 0;
}