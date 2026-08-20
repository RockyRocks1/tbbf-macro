#pragma once
#include <pixel/PixelAnalyzer.h>
#include <pixel/UDim2.h>
#include <tbbf/context/TbbfContext.h>

class ScreenStateReader {
public:
    static Rect GetHealthBarBounds(const FrameView& currentFrame) noexcept;
    static POINT GetInGameMenuAnchor(const FrameView& currentFrame) noexcept;
    static Rect GetWaveTextBounds(const FrameView& currentFrame, const POINT& inGameMenuAnchor) noexcept;
    static Rect GetExpBarBounds(const FrameView& currentFrame, const POINT& inGameMenuAnchor) noexcept;

    static SplashTextStatus GetSplashTextStatus(const FrameView& currentFrame) noexcept;
    static PlayerStatus GetPlayerStatus(PlayerStatus oldStatus, SplashTextStatus splashStatus, const TbbfContext::UiLayoutInfo& uiLayout, bool isInvincible, const FrameView& currentFrame) noexcept;
    static bool IsInGameMenuOpen(const FrameView& currentFrame, const Rect& expBarBounds) noexcept;
private:
    static bool IsDead(const FrameView& currentFrame, const Rect& healthBarBounds) noexcept;
    static bool IsDeployed(const FrameView& currentFrame, const Rect& expBarBounds) noexcept;
};
