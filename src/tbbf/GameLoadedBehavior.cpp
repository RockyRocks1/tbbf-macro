#include <tbbf/GameLoadedBehavior.h>
#include <tbbf/TbbfMacroInstance.h>

// WARNING: this requires setting roblox menu background opacity to zero
TickResult GameLoadedBehavior::OnTick(IMacroInstance* instance, const FrameView& frame) {
	TbbfMacroInstance* tbbfInstance = static_cast<TbbfMacroInstance*>(instance);
	if (!frame.data)
		return TickResult{ TickStatus::Yield, 16 };
	const Size2D clientSize = { frame.width, frame.height };
	static constexpr std::array<UDim2, 3> gameLoadedUDims = {
		UDim2(0.f, 38, 0.f, 32),
		UDim2(0.f, 20, 0.f, 32),
		UDim2(0.f, 56, 0.f, 32)
	};
	const int maxVariation = 1;
	static constexpr ColorRgba targetDefaultColor{ 0x12, 0x12, 0x15 };
	for (const UDim2& udim2 : gameLoadedUDims) {
		POINT pixelPosition = udim2.Resolve(clientSize);
		std::optional<ColorRgba> pixelColor = PixelAnalyzer::GetPixelColor(frame, pixelPosition);
		if (!pixelColor || !targetDefaultColor.IsCloseTo(*pixelColor, maxVariation))
			return TickResult{ TickStatus::Yield, 100 };
	}
	return TickResult{ TickStatus::Skipped, 16 };
}