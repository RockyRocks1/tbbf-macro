cmake_minimum_required(VERSION 3.20)
project(tbbf-macro LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# ImGui Dependency
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        v1.91.8
)
FetchContent_MakeAvailable(imgui)

set(IMGUI_TARGET_SOURCES
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp # demo: remove when done
    
    # Backends
    ${imgui_SOURCE_DIR}/backends/imgui_impl_win32.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_dx11.cpp
)

add_library(imgui STATIC ${IMGUI_TARGET_SOURCES} )
target_include_directories(imgui PUBLIC 
    ${imgui_SOURCE_DIR} 
    ${imgui_SOURCE_DIR}/backends
)

# Microsoft WIL Dependency
FetchContent_Declare(
    wil
    GIT_REPOSITORY https://github.com/microsoft/wil.git
    GIT_TAG        v1.0.260126.7
)
FetchContent_GetProperties(wil)
if(NOT wil_POPULATED)
    FetchContent_Populate(wil)
endif()

# curl Dependency
set(HTTP_ONLY ON CACHE INTERNAL "")
set(CURL_USE_SCHANNEL ON CACHE INTERNAL "")
set(CURL_USE_LIBPSL OFF CACHE INTERNAL "") 
set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "")
set(BUILD_TESTING OFF CACHE INTERNAL "")
set(BUILD_CURL_EXE OFF CACHE INTERNAL "")

FetchContent_Declare(
    curl
    GIT_REPOSITORY https://github.com/curl/curl.git
    GIT_TAG        curl-8_21_0
)
FetchContent_MakeAvailable(curl)

# nlohmann json Dependency
FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz
)
FetchContent_MakeAvailable(json)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/pixel/GdiPixelCapture.cpp 
    src/pixel/WgcPixelCapture.cpp
    src/pixel/PixelModifier.cpp
    src/pixel/PixelAnalyzer.cpp
    src/http/HttpClient.cpp
    src/roblox/RobloxWeb.cpp
    src/roblox/RobloxLauncher.cpp
    src/roblox/RobloxGame.cpp
    src/ocr/OcrTool.cpp
    src/utils/WindowUtils.cpp
    src/utils/ProcessUtils.cpp
)
target_include_directories(${PROJECT_NAME} PUBLIC
    ${wil_SOURCE_DIR}/include
    ${CURL_INCLUDE_DIRS}
    include
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    imgui
    d3d11
    dxgi
    dwmapi
    runtimeobject
    libcurl
    nlohmann_json::nlohmann_json
    windowsapp
)