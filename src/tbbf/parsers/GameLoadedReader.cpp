#include <tbbf/parsers/GameLoadedReader.h>

bool GameLoadedReader::IsGameLoaded(const FrameView& currentFrame) noexcept {
	static constexpr std::array<UDim2, 3> gameLoadedUDims = {
		UDim2(0.f, 38, 0.f, 32),
		UDim2(0.f, 20, 0.f, 32),
		UDim2(0.f, 56, 0.f, 32)
	};
	static constexpr ColorRgba targetDefaultColor{ 0x12, 0x12, 0x15 };
	constexpr int maxVariation = 1;
	
	const Size2D clientSize = { currentFrame.width, currentFrame.height };

	for (const UDim2& udim2 : gameLoadedUDims) {
		POINT pixelPosition = udim2.Resolve(clientSize);
		std::optional<ColorRgba> pixelColor = PixelAnalyzer::GetPixelColor(currentFrame, pixelPosition);
		if (!pixelColor || !targetDefaultColor.IsCloseTo(*pixelColor, maxVariation))
			return false;
	}
	return true;
}