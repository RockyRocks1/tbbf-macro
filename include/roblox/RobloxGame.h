#pragma once

#include <utils/WindowUtils.h>
#include <optional>
#include <memory>
#include <utility>
#include <pixel/IPixelCapture.h>
#include <pixel/GdiPixelCapture.h>
#include <pixel/WgcPixelCapture.h>

struct UDim2 {
    float scaleX = 0.0f, scaleY = 0.0f;
    int offsetX = 0, offsetY = 0;
    constexpr std::pair<int, int> Resolve(const Rect& bounds) const noexcept {
        return { 
            static_cast<int>(bounds.width * scaleX) + bounds.x + offsetX, 
            static_cast<int>(bounds.height * scaleY) + bounds.y + offsetY
        };
    }
};

class RobloxGame {
private:
	HWND m_hwnd;
    Rect m_clientBounds;
    std::unique_ptr<IPixelCapture> m_pixelCapture;
	RobloxGame(HWND hwnd);
public:
    static std::optional<RobloxGame> FromHwnd(HWND hwnd, PixelCaptureMode captureMode = PixelCaptureMode::WGC);
    static std::optional<RobloxGame> FromProcessId(DWORD processId, PixelCaptureMode captureMode = PixelCaptureMode::WGC);

    void UpdateClientBounds();
    void SetFocus();
    bool WasGameLoaded();
    bool WasDisconnected();

    inline HWND GetHwnd() const { return m_hwnd; }
    inline const Rect& GetClientBounds() const { return m_clientBounds; };

};