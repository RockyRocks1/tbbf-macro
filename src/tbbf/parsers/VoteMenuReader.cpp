#include <tbbf/parsers/VoteMenuReader.h>


std::optional<POINT> VoteMenuReader::GetSkipVotePosition(const FrameView& currentFrame) noexcept {
	static constexpr UDim2 skipVoteUDimPosition = UDim2(1.0f, -50, 1.0f, -75);
	static constexpr Size2D skipVoteSize{ 50, 25 };
	static constexpr ColorRgba targetWhiteColor = { 255, 255, 255 };

	const Size2D frameSize{ currentFrame.width, currentFrame.height };

	const POINT buttonPosition = skipVoteUDimPosition.Resolve(frameSize);
	const Rect skipVoteBounds{
		buttonPosition.x - skipVoteSize.width / 2,
		buttonPosition.y - skipVoteSize.width / 2,
		skipVoteSize.width,
		skipVoteSize.height
	};
	const FrameView& skipVoteCropped = PixelModifier::Crop(currentFrame, skipVoteBounds);
	std::optional<int> whitePixels = PixelAnalyzer::FindPixelOccurrences(skipVoteCropped, targetWhiteColor);

	if (!whitePixels || *whitePixels < 100)
		return std::nullopt;

	return buttonPosition;
}


