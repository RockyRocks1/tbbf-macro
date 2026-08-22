#pragma once
#include "FrameView.h"
#include <cmath>
#include <algorithm>

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
	// TODO: maybe use Rect for the parameter type?
	static FrameView Crop(const FrameView& sourceView, const Rect& cropRegion);

	static bool Grayscale(const FrameView& sourceView, FrameBuffer& destBuffer);
	static bool Invert(const FrameView& sourceView, FrameBuffer& destBuffer);
	static bool Threshold(const FrameView& sourceView, FrameBuffer& destBuffer, uint8_t thresholdVal, ThresholdType thresholdType);
	static bool Censor(const FrameView& sourceView, FrameBuffer& destBuffer, const Rect& censorRegion);
	static bool Upscale(const FrameView& sourceView, FrameBuffer& destBuffer, int scaleFactor);
	static bool Normalize(const FrameView& sourceView, FrameBuffer& destBuffer, const Rect& ignoreRegion = Rect{});
};