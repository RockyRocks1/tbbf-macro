#pragma once

#include <vector>
#include "IPixelCapture.h"

struct CoordPair {
	int x = 0;
	int y = 0;
};
class PixelAnalyzer {
public:
	PixelAnalyzer() = delete;

	static bool PixelSearch(CoordPair& outResult, const FrameView& frame, ColorRgba targetColor, int variation = 0);
	static std::vector<CoordPair> FindPixelOccurrences(const FrameView& frame, ColorRgba targetColor, int variation = 0);
	static bool BitmapSearch(CoordPair& outResult, const FrameView& frame, const FrameView& targetBitmap);
}