#include <tbbf/CentralContextBehavior.h>
#include <tbbf/TbbfMacroInstance.h>



SplashTextStatus CentralContextBehavior::GetSplashTextStatus(TbbfMacroInstance* instance, const FrameView& currentFrame) {
    static constexpr ColorRgba splashTextColor = ColorRgba{ 0xAA, 0xFF, 0xFF, 0xFF };
    static constexpr UDim2 splashTextStartUDim2 = UDim2(0.5f, -120, 0.917f, -25);
    static constexpr Size2D splashTextSize = { 240, 50 };

    POINT startPos = splashTextStartUDim2.Resolve(Size2D{currentFrame.width, currentFrame.height});

    FrameView cropped = PixelModifier::Crop(currentFrame, startPos.x, startPos.y, splashTextSize.width, splashTextSize.height);
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

TickStatus CentralContextBehavior::TickTbbf(TbbfMacroInstance* instance, TbbfCustomContext* context, const FrameView& currentFrame) {
    context->splashStatus = GetSplashTextStatus(instance, currentFrame);

	return TickStatus::Skipped;
}