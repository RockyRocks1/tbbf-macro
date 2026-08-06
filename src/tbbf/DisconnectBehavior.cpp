#include <tbbf/DisconnectBehavior.h>
#include <tbbf/TbbfMacroInstance.h>

TickResult DisconnectBehavior::OnTick(IMacroInstance* instance, const FrameView& frame) {
	TbbfMacroInstance* tbbfInstance = static_cast<TbbfMacroInstance*>(instance);
	if (!frame.data)
		return { TickStatus::Yield, 16 };
	const Size2D clientSize = { frame.width, frame.height };

	static constexpr std::array<UDim2, 4> disconnectUDims = {
		UDim2(0.5f, -199, 0.5f, -124),
		UDim2(0.5f, 199, 0.5f, -124),
		UDim2(0.5f, -199, 0.5f, 124),
		UDim2(0.5f, 199, 0.5f, 124)
	};
	static constexpr ColorRgba targetGrayColor{ 0x39, 0x3B, 0x3D };
	const int maxVariation = 1;
	for (const UDim2& udim2 : disconnectUDims) {
		POINT pixelPosition = udim2.Resolve(clientSize);
		std::optional<ColorRgba> pixelColor = PixelAnalyzer::GetPixelColor(frame, pixelPosition);
		if (!pixelColor || !targetGrayColor.IsCloseTo(*pixelColor, maxVariation))
			return { TickStatus::Skipped, 16 };
	}
	return { TickStatus::Terminated, 16 };
}