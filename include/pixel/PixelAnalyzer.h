#pragma once

#include <vector>
#include "IPixelCapture.h"

struct CoordPair {
	int x = 0;
	int y = 0;
};
struct Rect {
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;
};
struct FrameView {
    const uint8_t* data = nullptr;
    int width = 0;
    int height = 0;
    int stride = 0;

    static FrameView FromCapture(const IPixelCapture& capture) {
        const int width = capture.GetWidth();
        return FrameView{
            capture.GetBuffer(),
            width,
            capture.GetHeight(),
            width * 4
        };
    }
};

namespace PixelAnalyzer 
	bool PixelSearch(
        CoordPair& outResult, 
        const FrameView& frame,
        const Rect& searchRegion, 
        Color targetColor, 
        int variation = 0
    );
	std::vector<CoordPair> FindPixelOccurrences(
        const FrameView& frame,
        const Rect& searchRegion,
        Color targetColor,
        int variation = 0
    );
	bool BitmapSearch(
        CoordPair& outResult, 
        const FrameView& frame, 
        const Rect& searchRegion, 
        const FrameView& targetBitmap
    );
}