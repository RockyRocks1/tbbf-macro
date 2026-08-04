#pragma once

#include <vector>
#include "IPixelCapture.h"

struct CoordPair {
	uint32_t x = 0;
	uint32_t y = 0;
};
class PixelAnalyzer {
public:
	PixelAnalyzer() = delete;

	static bool GetPixelColor(ColorRgba& outResult, const FrameView& frame, uint32_t x, uint32_t y);
	static bool PixelSearch(CoordPair& outResult, const FrameView& frame, ColorRgba targetColor, int variation = 0);
	static std::vector<CoordPair> FindPixelOccurrences(const FrameView& frame, ColorRgba targetColor, int variation = 0);
	static bool BitmapSearch(CoordPair& outResult, const FrameView& frame, const FrameView& targetBitmap);
}