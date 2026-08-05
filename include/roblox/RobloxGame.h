#pragma once

#include <utils/WindowUtils.h>
#include <optional>
#include <memory>
#include <array>
#include <utility>
#include <pixel/IPixelCapture.h>
#include <pixel/GdiPixelCapture.h>
#include <pixel/WgcPixelCapture.h>
#include <pixel/PixelAnalyzer.h>

struct UDim2 {
    float scaleX = 0.0f;
    int offsetX = 0;
    float scaleY = 0.0f;
    int offsetY = 0;
    constexpr POINT Resolve(const Rect& bounds) const noexcept {
        const float posX = (static_cast<float>(bounds.width) * scaleX) + static_cast<float>(bounds.x + offsetX);
        const float posY = (static_cast<float>(bounds.height) * scaleY) + static_cast<float>(bounds.y + offsetY);
        return {
             static_cast<int>(posX >= 0.0f ? posX + 0.5f : posX - 0.5f),
             static_cast<int>(posY >= 0.0f ? posY + 0.5f : posY - 0.5f)
        };
    }
};

class RobloxGame {
private:
	HWND m_hwnd = nullptr;
    Rect m_clientBounds;
    std::unique_ptr<IPixelCapture> m_pixelCapture;
	RobloxGame(HWND hwnd);
public:
    static std::optional<RobloxGame> FromHwnd(HWND hwnd, PixelCaptureMode captureMode = PixelCaptureMode::WGC);
    static std::optional<RobloxGame> FromProcessId(DWORD processId, PixelCaptureMode captureMode = PixelCaptureMode::WGC);

    bool UpdateClientBounds();
    void SetFocus();
    
    bool WasGameLoaded() const;
    bool WasDisconnected() const;

    inline HWND GetHwnd() const { return m_hwnd; }
    inline bool WinExists() const { return WindowUtils::IsMainWindow(m_hwnd); }
    inline const Rect& GetClientBounds() const { return m_clientBounds; };
    IPixelCapture* GetPixelCapture() const { return m_pixelCapture.get(); };
};