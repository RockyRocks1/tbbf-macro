#pragma once
#include <pixel/UDim2.h>
#include <pixel/PixelAnalyzer.h>
#include <pixel/PixelModifier.h>

class VoteMenuReader {
public:
	static std::optional<POINT> GetSkipVotePosition(const FrameView& currentFrame) noexcept;
};


