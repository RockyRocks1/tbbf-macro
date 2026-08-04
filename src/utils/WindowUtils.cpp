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
    return GetWindow(hwnd, GW_OWNER) == nullptr && IsWindowVisible(hwnd);
}

HWND WindowUtils::FindMainWindow(DWORD processId) {
    HwndData data{ processId, nullptr };
    EnumWindows(EnumWindowsCallback, reinterpret_cast<LPARAM>(&data));
    return data.hwnd;
}

Rect WindowUtils::GetClientRectRelativeToWindow(HWND hwnd) {
    Rect bounds{};

    if (!IsWindow(hwnd))
        return bounds;

    RECT rectWindow, rectClient;
    GetWindowRect(hwnd, &rectWindow);
    GetClientRect(hwnd, &rectClient);

    POINT clientTopLeft{ 0, 0 };
    ClientToScreen(hwnd, &clientTopLeft);

    bounds.x = clientTopLeft.x - rectWindow.left;
    bounds.y = clientTopLeft.y - rectWindow.top;
    bounds.width = rectClient.right;
    bounds.height = rectClient.bottom;

    return bounds;
};