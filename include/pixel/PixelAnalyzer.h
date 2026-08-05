#pragma once

#include <vector>
#include <optional>
#include "IPixelCapture.h"


class PixelAnalyzer {
private:
	static bool CheckSanityOfAnalysis(const FrameView& frame, std::optional<POINT> coords) noexcept;
public:
	PixelAnalyzer() = delete;

	static std::optional<const ColorRgba> GetPixelColor(const FrameView& frame, const POINT& coords);
	static std::optional<POINT> PixelSearch(const FrameView& frame, ColorRgba targetColor, int variation = 0);
	static std::optional<POINT> BitmapSearch(const FrameView& frame, const FrameView& targetFrame);
};