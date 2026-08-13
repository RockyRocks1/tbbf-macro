#include <tbbf/CentralContextBehavior.h>
#include <tbbf/TbbfMacroInstance.h>

Rect CentralContextBehavior::GetHealthBarBounds(const FrameView& currentFrame) {
    static constexpr UDim2 healthBarStartUDim2(1.f, -141, 0.f, 24);
    static constexpr Size2D healthBarSize = { 124, 20 };
    POINT startPos = healthBarStartUDim2.Resolve(Size2D{ currentFrame.width, currentFrame.height });

    return Rect{ startPos.x, startPos.y, healthBarSize.width, healthBarSize.height };
}
POINT CentralContextBehavior::GetInGameMenuAnchor(const FrameView& currentFrame) {
    const int scaledX = 2 * currentFrame.height - 116;
    const int menuPosX = (std::min)(scaledX, currentFrame.width) - 5;
    const int menuPosY = currentFrame.height - 4 - 225;
    return POINT{ menuPosX, menuPosY };
}
Rect CentralContextBehavior::GetWaveTextBounds(const FrameView& currentFrame, POINT& inGameMenuAnchor) {
    static constexpr int offsetX = -190;
    static constexpr int offsetY = -60;
    static constexpr Size2D waveTextSize{ 110, 28 };

    return Rect{ inGameMenuAnchor.x + offsetX, inGameMenuAnchor.y + offsetY, waveTextSize.width, waveTextSize.height };
}
Rect CentralContextBehavior::GetExpBarBounds(const FrameView& currentFrame, POINT& inGameMenuAnchor) {
    static constexpr int offsetX = -424;
    static constexpr int offsetY = -28;
    static constexpr Size2D expBarSize{ 225, 24 };
    return Rect{ inGameMenuAnchor.x + offsetX, inGameMenuAnchor.y + offsetY, expBarSize.width, expBarSize.height };
}

SplashTextStatus CentralContextBehavior::GetSplashTextStatus(TbbfMacroInstance* instance, const FrameView& currentFrame) {
    static constexpr ColorRgba splashTextColor = ColorRgba{ 0xAA, 0xFF, 0xFF, 0xFF };
    static constexpr UDim2 splashTextStartUDim2(0.5f, -120, 0.917f, -25);
    static constexpr Size2D splashTextSize = { 240, 50 };

    POINT startPos = splashTextStartUDim2.Resolve(Size2D{currentFrame.width, currentFrame.height});
    Rect splashTextBounds{
        startPos.x,
        startPos.y,
        splashTextSize.width,
        splashTextSize.height
    };
    FrameView cropped = PixelModifier::Crop(currentFrame, splashTextBounds);
    std::optional<int> pixelOccurences = PixelAnalyzer::FindPixelOccurrences(cropped, splashTextColor);
    
    if (!pixelOccurences)
        return SplashTextStatus::Unknown;

    int count = *pixelOccurences;

    if (count > 2000)
        return SplashTextStatus::Welcome;
    if (count > 300) 
        return SplashTextStatus::Vote;

    return SplashTextStatus::Unknown;
}

PlayerStatus CentralContextBehavior::GetPlayerStatus(TbbfMacroInstance* instance, TbbfCustomContext* context, const FrameView& currentFrame) {
    const PlayerStatus playerStatus = context->playerStatus;

    if (playerStatus == PlayerStatus::Loading)
        return PlayerStatus::Loading;

    if (context->splashStatus == SplashTextStatus::Welcome)
        return PlayerStatus::Menu;

    if (IsDeployed(currentFrame, context->uiLayout.expBarBounds)) {
        if (playerStatus == PlayerStatus::Menu) {
            context->lastRespawnTick = instance->GetCurrentTimestamp();
            context->inGameMenuOpen = false;
            context->toolEquipped = false;
            return PlayerStatus::Deployed;
        }
        bool isInvincible = HasInvincibility(instance->GetCurrentTimestamp(), context->lastRespawnTick);
        if (!isInvincible && IsDead(currentFrame, context->uiLayout.healthBarBounds) )
            return PlayerStatus::Dead;

        return PlayerStatus::Deployed;
    }
    return playerStatus;
}
bool CentralContextBehavior::IsDead(const FrameView& currentFrame, const Rect& healthBarBounds) {
    constexpr uint8_t aliveSaturationThreshold = 100;
    int centerY = healthBarBounds.y + (healthBarBounds.height / 2);
    int startX = (healthBarBounds.x + healthBarBounds.width) - 5;
    int endX = healthBarBounds.x + 5;

    for (int x = startX; x >= endX; --x) {
        uint8_t saturation = *PixelAnalyzer::GetSaturation(currentFrame, POINT{x, centerY});

        if (saturation >= aliveSaturationThreshold)
            return false;
    }

    return true;
}
bool CentralContextBehavior::IsDeployed(const FrameView& currentFrame, const Rect& expBarBounds) {
    static constexpr ColorRgba mainBarColor{ 0xDC, 0xCD, 00 };
    static constexpr ColorRgba barBackColor{ 0x41, 0x3D, 00 };
    static constexpr int variation = 3;

    const Rect searchRegion{
        expBarBounds.x + 1,
        expBarBounds.y,
        expBarBounds.width,
        currentFrame.height - expBarBounds.y - 1
    };
    FrameView expBarFrame = PixelModifier::Crop(currentFrame, searchRegion);

    if (expBarFrame.data == nullptr)
        return false;
    
    return PixelAnalyzer::PixelSearch(expBarFrame, mainBarColor, variation) || PixelAnalyzer::PixelSearch(expBarFrame, barBackColor, variation);
}

bool CentralContextBehavior::HasInvincibility(uint64_t currentTick, uint64_t lastRespawnTick) {
    static constexpr uint64_t invincibilityTime = 4000;
    return (currentTick - lastRespawnTick) < invincibilityTime;
}
TickStatus CentralContextBehavior::TickTbbf(TbbfMacroInstance* instance, TbbfCustomContext* context, const FrameView& currentFrame) {
    UiLayoutInfo& uiLayout = context->uiLayout;
    if (uiLayout.viewportSize.width != currentFrame.width || uiLayout.viewportSize.height != currentFrame.height) {
        uiLayout.viewportSize.width = currentFrame.width;
        uiLayout.viewportSize.height = currentFrame.height;

        uiLayout.inGameMenuOpenOffset = 225;
        uiLayout.inGameMenuAnchor = GetInGameMenuAnchor(currentFrame);
        uiLayout.healthBarBounds = GetHealthBarBounds(currentFrame);
        uiLayout.waveTextBounds = GetWaveTextBounds(currentFrame, uiLayout.inGameMenuAnchor);
        uiLayout.expBarBounds = GetExpBarBounds(currentFrame, uiLayout.inGameMenuAnchor);
    }
    context->splashStatus = GetSplashTextStatus(instance, currentFrame);
    context->playerStatus = GetPlayerStatus(instance, context, currentFrame);

	return TickStatus::Skipped;
}