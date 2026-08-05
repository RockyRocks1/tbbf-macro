#include <pixel/PixelAnalyzer.h>

bool PixelAnalyzer::CheckSanityOfAnalysis(const FrameView& frame, std::optional<POINT> coords) noexcept {
	if (!frame.data || frame.format != PixelFormat::Bgra8)
		return false;

	if (coords)
		return coords->x >= 0 && coords->x < frame.width &&
			coords->y >= 0 && coords->y < frame.height;

	return true;
}


std::optional<ColorRgba> PixelAnalyzer::GetPixelColor(const FrameView& frame, const POINT& coords) {
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
		const uint8_t* pRow = static_cast<const uint8_t*>(frame.data + frame.stride * y);
		const uint8_t* pPixel = pRow;

		for (int x = 0; x < frame.width; x++) {
			if (abs(targetColor.r - pPixel[idxR]) <= variation && 
				abs(targetColor.g - pPixel[idxG]) <= variation && 
				abs(targetColor.b - pPixel[idxB]) <= variation)
				return POINT{ x, y };
			pPixel += bytesPerPixel;
		}
	}
	return std::nullopt;
}