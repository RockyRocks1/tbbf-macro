#pragma once

#include <vector>
#include <optional>
#include "IPixelCapture.h"


class PixelAnalyzer {
private:
	static bool CheckSanityOfAnalysis(const FrameView& frame, std::optional<POINT> coords, PixelFormat expectedFormat = PixelFormat::Bgra8) noexcept;
public:
	PixelAnalyzer() = delete;

	static std::optional<const ColorRgba> GetPixelColor(const FrameView& frame, const POINT& coords);
	static std::optional<POINT> PixelSearch(const FrameView& frame, ColorRgba targetColor, int variation = 0);
	static std::optional<POINT> PixelSearchBackwards(const FrameView& frame, ColorRgba targetColor, int variation = 0);
	static std::optional<int> FindPixelOccurrences(const FrameView& frame, ColorRgba targetColor, int variation = 0);
	static std::optional<POINT> BitmapSearch(const FrameView& frame, const FrameView& targetFrame);

	static std::optional<uint8_t> GetSaturation(const FrameView& frame, const POINT& coords);
	static std::optional<uint8_t> GetLuminance(const FrameView& frame, const POINT& coords);
	static std::optional<uint8_t> GetMaxLuminance(const FrameView& grayscaleFrame);
};