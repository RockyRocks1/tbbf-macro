#include <tbbf/parsers/DisconnectReader.h>

bool DisconnectReader::IsDisconnected(const FrameView& currentFrame) noexcept {
	static constexpr std::array<UDim2, 4> disconnectUDims = {
		UDim2(0.5f, -199, 0.5f, -124),
		UDim2(0.5f, 199, 0.5f, -124),
		UDim2(0.5f, -199, 0.5f, 124),
		UDim2(0.5f, 199, 0.5f, 124)
	};
	static constexpr ColorRgba targetGrayColor{ 0x39, 0x3B, 0x3D };
	static constexpr int maxVariation = 1;

	const Size2D clientSize = { currentFrame.width, currentFrame.height };

	for (const UDim2& udim2 : disconnectUDims) {
		POINT pixelPosition = udim2.Resolve(clientSize);
		std::optional<ColorRgba> pixelColor = PixelAnalyzer::GetPixelColor(currentFrame, pixelPosition);
		if (!pixelColor || !targetGrayColor.IsCloseTo(*pixelColor, maxVariation))
			return false;
	}
	return true;
};