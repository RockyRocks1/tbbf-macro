#include <utils/WindowUtils.h>

namespace { // prevents name conflicts
    struct HwndData {
        DWORD processId = 0;
        HWND hwnd = nullptr;
    };

    BOOL CALLBACK EnumWindowsCallback(HWND hwnd, LPARAM lParam) {
        HwndData& data = *reinterpret_cast<HwndData*>(lParam);
        DWORD processId = 0;
        GetWindowThreadProcessId(hwnd, &processId);

        if (data.processId == processId && WindowUtils::IsMainWindow(hwnd)) {
            data.hwnd = hwnd;
            return FALSE;
        }
        return TRUE;
    }
}

bool WindowUtils::IsMainWindow(HWND hwnd) {
    return !GetWindow(hwnd, GW_OWNER) && IsWindowVisible(hwnd);
}

HWND WindowUtils::FindMainWindow(DWORD processId) {
    HwndData data{ processId, nullptr };
    EnumWindows(::EnumWindowsCallback, reinterpret_cast<LPARAM>(&data));
    return data.hwnd;
}

std::optional<POINT> WindowUtils::GetClientOffsetFromWindow(HWND hwnd) {
    Rect bounds{};
    
    if (!IsWindow(hwnd))
        return std::nullopt;

    RECT rectWindow;
    if (!GetWindowRect(hwnd, &rectWindow))
        return std::nullopt;

    POINT clientTopLeft{ 0, 0 };
    if (!ClientToScreen(hwnd, &clientTopLeft))
        return std::nullopt;

    return POINT{
        clientTopLeft.x - rectWindow.left,
        clientTopLeft.y - rectWindow.top
    };
};
std::optional<POINT> WindowUtils::GetClientOffsetFromWgc(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd))
        return std::nullopt;

    POINT offset{ 0, 0 };

    if (!ClientToScreen(hwnd, &offset)) 
        return std::nullopt;

    RECT wgcRect{};
    if (FAILED(DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &wgcRect, sizeof(wgcRect))))
        return std::nullopt;

    offset.x -= wgcRect.left;
    offset.y -= wgcRect.top;
    return offset;
}

bool WindowUtils::SetAspectRatio(HWND hwnd, int targetWidth, AspectRatio ratio) {
    if (!hwnd || !IsWindow(hwnd))
        return false;

    double targetAspectRatio;
    switch (ratio) {
    case AspectRatio::Widescreen:
        targetAspectRatio = 16.0 / 9.0;
        break;
    case AspectRatio::StandardDefinition:
    default:
        targetAspectRatio = 4.0 / 3.0;
        break;
    }
    int targetHeight = static_cast<int>(std::round(targetWidth / targetAspectRatio));

    RECT rcClient, rcWindow;
    GetClientRect(hwnd, &rcClient);
    GetWindowRect(hwnd, &rcWindow);

    int borderWidth = (rcWindow.right - rcWindow.left) - rcClient.right;
    int borderHeight = (rcWindow.bottom - rcWindow.top) - rcClient.bottom;

    return MoveWindow(hwnd, rcWindow.left, rcWindow.top, targetWidth + borderWidth, targetHeight + borderHeight, TRUE);
}


std::optional<POINT> WindowUtils::ToAbsoluteCoordinates(const POINT& screenPosition) {
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int normalX = (screenPosition.x * 65535) / screenWidth;
    int normalY = (screenPosition.y * 65535) / screenHeight;
    if (normalX > 65535 || normalY > 65535)
        return std::nullopt;
    return POINT{ normalX, normalY };
}