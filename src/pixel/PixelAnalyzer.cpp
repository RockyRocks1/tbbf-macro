#include <pixel/PixelAnalyzer.h>

bool PixelAnalyzer::CheckSanityOfAnalysis(const FrameView& frame, std::optional<POINT> coords, PixelFormat expectedFormat) noexcept {
	if (!frame.data || frame.format != expectedFormat)
		return false;

	if (coords)
		return coords->x >= 0 && coords->x < frame.width &&
			coords->y >= 0 && coords->y < frame.height;

	return true;
}


std::optional<const ColorRgba> PixelAnalyzer::GetPixelColor(const FrameView& frame, const POINT& coords) {
	if (!CheckSanityOfAnalysis(frame, coords))
		return std::nullopt;

	const uint8_t bytesPerPixel = 4;
	const size_t index = frame.stride * coords.y + static_cast<size_t>(coords.x * bytesPerPixel);

	return ColorRgba{
		frame.data[index + static_cast<size_t>(BgraChannel::R)],
		frame.data[index + static_cast<size_t>(BgraChannel::G)],
		frame.data[index + static_cast<size_t>(BgraChannel::B)],
		frame.data[index + static_cast<size_t>(BgraChannel::A)]
	};
};

std::optional<POINT> PixelAnalyzer::PixelSearch(const FrameView& frame, ColorRgba targetColor, int variation) {
	if (!CheckSanityOfAnalysis(frame, std::nullopt))
		return std::nullopt;
	
	const int bytesPerPixel = 4;
	const size_t idxR = static_cast<size_t>(BgraChannel::R);
	const size_t idxG = static_cast<size_t>(BgraChannel::G);
	const size_t idxB = static_cast<size_t>(BgraChannel::B);
	for (int y = 0; y < frame.height; y++) {
		const ColorBgra* pRow = reinterpret_cast<const ColorBgra*>(frame.data.get() + frame.stride * y);

		for (int x = 0; x < frame.width; x++) {
			if (targetColor.IsCloseTo(pRow[x], variation))
				return POINT{ x, y };
		}
	}
	return std::nullopt;
}
std::optional<int> PixelAnalyzer::FindPixelOccurrences(const FrameView& frame, ColorRgba targetColor, int variation) {
	if (!CheckSanityOfAnalysis(frame, std::nullopt))
		return std::nullopt;

	int occurences = 0;
	const int bytesPerPixel = 4;
	const size_t idxR = static_cast<size_t>(BgraChannel::R);
	const size_t idxG = static_cast<size_t>(BgraChannel::G);
	const size_t idxB = static_cast<size_t>(BgraChannel::B);
	for (int y = 0; y < frame.height; y++) {
		const ColorBgra* pRow = reinterpret_cast<const ColorBgra*>(frame.data.get() + frame.stride * y);

		for (int x = 0; x < frame.width; x++) {
			if (targetColor.IsCloseTo(pRow[x], variation))
				occurences++;
		}
	}
	return occurences;
}


std::optional<POINT> PixelAnalyzer::PixelSearchBackwards(const FrameView& frame, ColorRgba targetColor, int variation) {
	if (!CheckSanityOfAnalysis(frame, std::nullopt))
		return std::nullopt;

	const int bytesPerPixel = 4;
	const size_t idxR = static_cast<size_t>(BgraChannel::R);
	const size_t idxG = static_cast<size_t>(BgraChannel::G);
	const size_t idxB = static_cast<size_t>(BgraChannel::B);
	for (int y = frame.height - 1; y >= 0; y--) {
		const ColorBgra* pRow = reinterpret_cast<const ColorBgra*>(frame.data.get() + frame.stride * y);

		for (int x = frame.width - 1; x >= 0; x--) {
			if (targetColor.IsCloseTo(pRow[x], variation))
				return POINT{ x, y };
		}
	}
	return std::nullopt;
}

std::optional<uint8_t> PixelAnalyzer::GetMaxLuminance(const FrameView& grayscaleFrame) {
	if (!CheckSanityOfAnalysis(grayscaleFrame, std::nullopt, PixelFormat::Gray8))
		return std::nullopt;

	const int bytesPerPixel = 1;
	uint8_t maxLuminance = 0;
	for (int y = 0; y < grayscaleFrame.height; y++) {
		const uint8_t* pRow = grayscaleFrame.data.get() + (grayscaleFrame.stride * y);

		for (int x = 0; x < grayscaleFrame.width; x++) {
			if (pRow[x] > maxLuminance)
				maxLuminance = pRow[x];
		}
	}
	return maxLuminance;
}