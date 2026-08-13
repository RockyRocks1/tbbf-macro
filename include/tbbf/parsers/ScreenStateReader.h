#pragma once
#include <pixel/FrameView.h>
#include <pixel/Rect.h>
#include <pixel/UDim2.h>
#include <tbbf/context/TbbfContext.h>

class ScreenStateReader {
public:
    static Rect GetHealthBarBounds(const FrameView& currentFrame) noexcept;
    static POINT GetInGameMenuAnchor(const FrameView& currentFrame) noexcept;
    static Rect GetWaveTextBounds(const FrameView& currentFrame, const POINT& inGameMenuAnchor) noexcept;
    static Rect GetExpBarBounds(const FrameView& currentFrame, const POINT& inGameMenuAnchor) noexcept;

    static SplashTextStatus GetSplashTextStatus(const FrameView& currentFrame) noexcept;
    static PlayerStatus GetPlayerStatus(PlayerStatus currentStatus, SplashTextStatus splashStatus, const TbbfContext::UiLayoutInfo& uiLayout, uint64_t currentTimestamp, uint64_t lastRespawnTick, const FrameView& currentFrame) noexcept;
private:
    static bool IsDead(const FrameView& currentFrame, const Rect& healthBarBounds) noexcept;
    static bool IsDeployed(const FrameView& currentFrame, const Rect& expBarBounds) noexcept;
    static bool HasInvincibility(uint64_t currentTick, uint64_t lastRespawnTick) noexcept;
};
