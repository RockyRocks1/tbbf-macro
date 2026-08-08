#include <tbbf/DisconnectBehavior.h>
#include <tbbf/TbbfMacroInstance.h>

TickStatus DisconnectBehavior::TickTbbf(TbbfMacroInstance* instance, TbbfCustomContext* context, const FrameView& currentFrame) {
	const Size2D clientSize = { currentFrame.width, currentFrame.height };

	static constexpr std::array<UDim2, 4> disconnectUDims = {
		UDim2(0.5f, -199, 0.5f, -124),
		UDim2(0.5f, 199, 0.5f, -124),
		UDim2(0.5f, -199, 0.5f, 124),
		UDim2(0.5f, 199, 0.5f, 124)
	};
	static constexpr ColorRgba targetGrayColor{ 0x39, 0x3B, 0x3D };
	constexpr int maxVariation = 1;
	for (const UDim2& udim2 : disconnectUDims) {
		POINT pixelPosition = udim2.Resolve(clientSize);
		std::optional<ColorRgba> pixelColor = PixelAnalyzer::GetPixelColor(currentFrame, pixelPosition);
		if (!pixelColor || !targetGrayColor.IsCloseTo(*pixelColor, maxVariation))
			return TickStatus::Skipped;
	}
	return TickStatus::Terminated;
}