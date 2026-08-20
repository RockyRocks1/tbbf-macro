#include <tbbf/parsers/WaveTextReader.h>

std::optional<int> WaveTextReader::ReadFromFrame(const FrameView& currentFrame, const Rect& waveTextBounds) noexcept {
	FrameView croppedWaveText = PixelModifier::Crop(currentFrame, waveTextBounds);
	if (!croppedWaveText.data)
		return std::nullopt;

	if (!WaveTextExists(croppedWaveText))
		return std::nullopt;


	std::string rawWaveText = ParseWaveText(croppedWaveText);
	if (rawWaveText.find("Wave") == std::string::npos)
		return std::nullopt;

	return SanitizeWaveText(rawWaveText);
};
bool WaveTextReader::WaveTextExists(const FrameView& waveTextFrame) noexcept {
	static constexpr ColorRgba whiteText{ 255, 255, 255 };
	static constexpr int minimumWavePixelCount = 10; // TEMPORARY
	std::optional<int> pixelOccurrences = PixelAnalyzer::FindPixelOccurrences(waveTextFrame, whiteText);

	return pixelOccurrences && *pixelOccurrences > minimumWavePixelCount;
};
std::string WaveTextReader::ParseWaveText(const FrameView& waveTextFrame) noexcept {
	static constexpr uint8_t thresholdTextValue = 180;
	FrameBuffer grayscaleBuffer;
	PixelModifier::Grayscale(waveTextFrame, grayscaleBuffer);
	FrameBuffer thresholdBuffer;
	PixelModifier::Threshold(grayscaleBuffer.GetView(), thresholdBuffer, thresholdTextValue, ThresholdType::BINARY);
	FrameBuffer upscaleBuffer;
	PixelModifier::Upscale(thresholdBuffer.GetView(), upscaleBuffer, 2);
	std::string rawWaveText = OcrTool::RecognizeText(upscaleBuffer.GetView());

	return rawWaveText;
};
std::optional<int> WaveTextReader::SanitizeWaveText(std::string rawWaveText) noexcept {
	std::string digitString = "";
	for (char c : rawWaveText) {
		if (std::isdigit(static_cast<unsigned char>(c)))
			digitString += c;
	}

	if (digitString.empty())
		return std::nullopt;

	return std::stol(digitString);
};