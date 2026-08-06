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


class RobloxGame {
private:
	HWND m_hwnd = nullptr;
    mutable Size2D m_clientBounds;
    PixelCaptureMode m_captureMode;
    std::unique_ptr<IPixelCapture> m_pixelCapture;
	RobloxGame(HWND hwnd, PixelCaptureMode captureMode);

    bool UpdateClientBounds() const;
public:
    ~RobloxGame() = default;
    static std::unique_ptr<RobloxGame> FromHwnd(HWND hwnd, PixelCaptureMode captureMode = PixelCaptureMode::WGC);
    static std::unique_ptr<RobloxGame> FromProcessId(DWORD processId, PixelCaptureMode captureMode = PixelCaptureMode::WGC);

    FrameView GetLatestFrame() const;
    void SetFocus();
    
    inline HWND GetHwnd() const { return m_hwnd; }
    inline bool WinExists() const { return WindowUtils::IsMainWindow(m_hwnd); }
    inline const Size2D& GetClientBounds() const { return m_clientBounds; };
};