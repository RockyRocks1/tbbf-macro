#include <tbbf/GameLoadedBehavior.h>
#include <tbbf/TbbfMacroInstance.h>

// WARNING: this requires setting roblox menu background opacity to zero
TickStatus GameLoadedBehavior::TickTbbf(TbbfMacroInstance* instance, TbbfCustomContext* context, const FrameView& currentFrame) {
	const Size2D clientSize = { currentFrame.width, currentFrame.height };
	static constexpr std::array<UDim2, 3> gameLoadedUDims = {
		UDim2(0.f, 38, 0.f, 32),
		UDim2(0.f, 20, 0.f, 32),
		UDim2(0.f, 56, 0.f, 32)
	};
	constexpr int maxVariation = 1;
	static constexpr ColorRgba targetDefaultColor{ 0x12, 0x12, 0x15 };
	for (const UDim2& udim2 : gameLoadedUDims) {
		POINT pixelPosition = udim2.Resolve(clientSize);
		std::optional<ColorRgba> pixelColor = PixelAnalyzer::GetPixelColor(currentFrame, pixelPosition);
		if (!pixelColor || !targetDefaultColor.IsCloseTo(*pixelColor, maxVariation))
			return TickStatus::Yield;
	}
	return TickStatus::Skipped;
}