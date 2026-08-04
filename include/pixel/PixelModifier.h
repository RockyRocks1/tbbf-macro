#pragma once
#include "FrameView.h"
#include <cmath>

enum class ThresholdType {
	BINARY,
	BINARY_INV,
	TRUNC,
	TO_ZERO,
	TO_ZERO_INV
};

class PixelModifier {
private:
	template<typename Func>
	static bool Map1to1(const FrameView& sourceView, FrameBuffer& destBuffer, Func iteratorFunction);
public:	
	PixelModifier() = delete;
	static FrameView Crop(const FrameView& sourceView, uint32_t x, uint32_t y, uint32_t width, uint32_t height);


	static bool Grayscale(const FrameView& sourceView, FrameBuffer& destBuffer);
	static bool Invert(const FrameView& sourceView, FrameBuffer& destBuffer);
	static bool Threshold(const FrameView& sourceView, FrameBuffer& destBuffer, uint8_t thresholdVal, ThresholdType thresholdType);

};