#pragma once
#include <pixel/UDim2.h>
#include <pixel/FrameView.h>

class VoteMenuReader {
public:
	static std::optional<POINT> FindSkipVote(const FrameView& currentFrame) noexcept;
};


