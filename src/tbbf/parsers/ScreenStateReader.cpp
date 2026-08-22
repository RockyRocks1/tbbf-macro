#include <tbbf/parsers/ScreenStateReader.h>


Rect ScreenStateReader::GetHealthBarBounds(const FrameView& currentFrame) noexcept {
    static constexpr UDim2 healthBarStartUDim2(1.f, -141, 0.f, 24);
    static constexpr Size2D healthBarSize = { 124, 20 };
    POINT startPos = healthBarStartUDim2.Resolve(Size2D{ currentFrame.width, currentFrame.height });

    return Rect{ startPos.x, startPos.y, healthBarSize.width, healthBarSize.height };
};
POINT ScreenStateReader::GetInGameMenuAnchor(const FrameView& currentFrame) noexcept {
    const int scaledX = 2 * currentFrame.height - 116;
    const int menuPosX = (std::min)(scaledX, currentFrame.width) - 5;
    const int menuPosY = currentFrame.height - 4 - 225;
    return POINT{ menuPosX, menuPosY };
};
bool ScreenStateReader::IsInGameMenuOpen(const FrameView& currentFrame, const Rect& expBarBounds) noexcept {
    static constexpr ColorRgba mainBarColor{ 0xDC, 0xCD, 00 };
    static constexpr ColorRgba barBackColor{ 0x41, 0x3D, 00 };
    static constexpr int variation = 3;

    FrameView expBarFrame = PixelModifier::Crop(currentFrame, expBarBounds);

    if (expBarFrame.data == nullptr)
        return false;

    return PixelAnalyzer::PixelSearch(expBarFrame, mainBarColor, variation) || PixelAnalyzer::PixelSearch(expBarFrame, barBackColor, variation);
}
Rect ScreenStateReader::GetWaveTextBounds(const FrameView& currentFrame, const POINT& inGameMenuAnchor) noexcept {
    static constexpr int offsetX = -190;
    static constexpr int offsetY = -60;
    static constexpr Size2D waveTextSize{ 110, 28 };

    return Rect{ inGameMenuAnchor.x + offsetX, inGameMenuAnchor.y + offsetY, waveTextSize.width, waveTextSize.height };
};
Rect ScreenStateReader::GetExpBarBounds(const FrameView& currentFrame, const POINT& inGameMenuAnchor) noexcept {
    static constexpr int offsetX = -424;
    static constexpr int offsetY = -28;
    static constexpr Size2D expBarSize{ 225, 24 };
    return Rect{ inGameMenuAnchor.x + offsetX, inGameMenuAnchor.y + offsetY, expBarSize.width, expBarSize.height };
};
SplashTextStatus ScreenStateReader::GetSplashTextStatus(const FrameView& currentFrame) noexcept {
    static constexpr ColorRgba splashTextColor = ColorRgba{ 0xAA, 0xFF, 0xFF, 0xFF };
    static constexpr UDim2 splashTextStartUDim2(0.5f, -120, 0.917f, -25);
    static constexpr Size2D splashTextSize = { 240, 50 };

    POINT startPos = splashTextStartUDim2.Resolve(Size2D{ currentFrame.width, currentFrame.height });
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
};
PlayerStatus ScreenStateReader::GetPlayerStatus(PlayerStatus oldStatus, SplashTextStatus splashStatus, const TbbfContext::UiLayoutInfo& uiLayout, bool isInvincible, const FrameView& currentFrame) noexcept {
    if (splashStatus == SplashTextStatus::Vote)
        return PlayerStatus::Loading;

    if (splashStatus == SplashTextStatus::Welcome)
        return PlayerStatus::Menu;

    if (IsDeployed(currentFrame, uiLayout.expBarBounds)) {
        if (!isInvincible && IsDead(currentFrame, uiLayout.healthBarBounds))
            return PlayerStatus::Dead;

        return PlayerStatus::Deployed;
    }
    return oldStatus;
};
bool ScreenStateReader::IsDead(const FrameView& currentFrame, const Rect& healthBarBounds) noexcept {
    const int centerY = healthBarBounds.y + (healthBarBounds.height / 2);

    const int centerX = healthBarBounds.x + (healthBarBounds.width / 2);
    POINT midPoint{ centerX, centerY };

    uint8_t centerSaturation = *PixelAnalyzer::GetSaturation(currentFrame, midPoint);
    uint8_t centerBrightness = *PixelAnalyzer::GetLuminance(currentFrame, midPoint);


    constexpr uint8_t minUiVisibleBrightness = 70;
    if (centerBrightness < minUiVisibleBrightness)
        return false;


    const int leftX = healthBarBounds.x + 5;
    uint8_t leftSaturation = *PixelAnalyzer::GetSaturation(currentFrame, POINT{ leftX, centerY });

    constexpr uint8_t aliveSaturationThreshold = 130;
    return leftSaturation < aliveSaturationThreshold;
}
bool ScreenStateReader::IsDeployed(const FrameView& currentFrame, const Rect& expBarBounds) noexcept {
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
};
