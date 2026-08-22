#include <tbbf/parsers/ToolSlotReader.h>

const Rect ToolSlotReader::GetSlotRect(const FrameView& currentFrame) noexcept {
	const Size2D frameSize{ currentFrame.width, currentFrame.height };
	const POINT toolFramePosition = slotOneUDim.Resolve(frameSize);
	return Rect{
		toolFramePosition.x - slotSize.width / 2,
		toolFramePosition.y - slotSize.height / 2,
		slotSize.width,
		slotSize.height
	};
}

bool ToolSlotReader::IsSlotExists(const FrameView& currentFrame) noexcept {
	static constexpr ColorRgba toolFrameColor{ 0x1F, 0x1F, 0x1F };
	const FrameView toolFrameCropped = PixelModifier::Crop(currentFrame, GetSlotRect(currentFrame));
	
	std::optional<int> pixelOccurences = PixelAnalyzer::FindPixelOccurrences(toolFrameCropped, toolFrameColor);

	if (!pixelOccurences)
		return false;

	return *pixelOccurences > 30; // TEMPORARY
};
bool ToolSlotReader::IsSlotActive(const FrameView& currentFrame) noexcept {
	static constexpr ColorRgba activeColor{ 0x5A, 0x8E, 0xE9 };
	const FrameView toolFrameCropped = PixelModifier::Crop(currentFrame, GetSlotRect(currentFrame));

	std::optional<int> pixelOccurences = PixelAnalyzer::FindPixelOccurrences(toolFrameCropped, activeColor);

	if (!pixelOccurences)
		return false;

	return *pixelOccurences > 30; // also TEMPORARY
};
