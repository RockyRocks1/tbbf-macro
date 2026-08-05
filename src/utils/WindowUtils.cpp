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
