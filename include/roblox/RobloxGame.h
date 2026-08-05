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
#include <pixel/PixelModifier.h>

struct UDim2 {
    float scaleX = 0.0f;
    int offsetX = 0;
    float scaleY = 0.0f;
    int offsetY = 0;
    constexpr POINT Resolve(const Size2D& bounds) const noexcept {
        const float posX = (static_cast<float>(bounds.width) * scaleX) + static_cast<float>(offsetX);
        const float posY = (static_cast<float>(bounds.height) * scaleY) + static_cast<float>(offsetY);
        return {
             static_cast<int>(posX >= 0.0f ? posX + 0.5f : posX - 0.5f),
             static_cast<int>(posY >= 0.0f ? posY + 0.5f : posY - 0.5f)
        };
    }
};

class RobloxGame {
private:
	HWND m_hwnd = nullptr;
    mutable Size2D m_clientBounds;
    PixelCaptureMode m_captureMode;
    std::unique_ptr<IPixelCapture> m_pixelCapture;
	RobloxGame(HWND hwnd, PixelCaptureMode captureMode);

    bool UpdateClientBounds() const;
public:
    static std::optional<RobloxGame> FromHwnd(HWND hwnd, PixelCaptureMode captureMode = PixelCaptureMode::WGC);
    static std::optional<RobloxGame> FromProcessId(DWORD processId, PixelCaptureMode captureMode = PixelCaptureMode::WGC);

    FrameView GetLatestFrame() const;
    void SetFocus();
    
    bool WasGameLoaded(const FrameView& currentFrame) const;
    bool WasDisconnected(const FrameView& currentFrame) const;

    inline HWND GetHwnd() const { return m_hwnd; }
    inline bool WinExists() const { return WindowUtils::IsMainWindow(m_hwnd); }
    inline const Size2D& GetClientBounds() const { return m_clientBounds; };
};