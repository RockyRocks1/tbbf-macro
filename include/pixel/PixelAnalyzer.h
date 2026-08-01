#pragma once
#include <vector>
#include "IPixelCapture.h"

struct CoordPair {
	int x;
	int y;
};
class PixelAnalyzer {
private:
	const IPixelCapture& m_capture;
public:
	PixelAnalyzer(const IPixelCapture& capture) : m_capture(capture) {};

	bool PixelSearch(CoordPair& outResult, int relX, int relY, int width, int height, Color targetColor, int variation = 0) const;
	std::vector<CoordPair> FindPixelOccurrences(int relX, int relY, int width, int height, Color targetColor, int variation = 0) const;
	bool BitmapSearch(CoordPair& outResult, int relX, int relY, int width, int height, const uint8_t* targetBitmap) const;
};