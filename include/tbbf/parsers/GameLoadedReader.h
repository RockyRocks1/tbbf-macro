#pragma once
#include <pixel/UDim2.h>
#include <pixel/PixelAnalyzer.h>
#include <array>

class GameLoadedReader {
public:
	static bool IsGameLoaded(const FrameView& currentFrame) noexcept;
};


