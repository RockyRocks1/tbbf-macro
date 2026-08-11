#include <tbbf/WaveChangedBehavior.h>
#include <tbbf/TbbfMacroInstance.h>
#include <iostream>

bool WaveChangedBehavior::WaveTextExists(const FrameView& waveTextFrame) {
	static constexpr ColorRgba whiteText{ 255, 255, 255 };
	static constexpr int minimumWavePixelCount = 10; // TEMPORARY
	std::optional<int> pixelOccurrences = PixelAnalyzer::FindPixelOccurrences(waveTextFrame, whiteText);

	return pixelOccurrences && *pixelOccurrences > minimumWavePixelCount;
}

std::string WaveChangedBehavior::ParseWaveText(const FrameView& waveTextFrame) {
	static constexpr uint8_t thresholdTextValue = 180;
	FrameBuffer grayscaleBuffer;
	PixelModifier::Grayscale(waveTextFrame, grayscaleBuffer);
	FrameBuffer thresholdBuffer;
	PixelModifier::Threshold(grayscaleBuffer.GetView(), thresholdBuffer, thresholdTextValue, ThresholdType::BINARY);
	FrameBuffer upscaleBuffer;
	PixelModifier::Upscale(thresholdBuffer.GetView(), upscaleBuffer, 2);
	std::string rawWaveText = OcrTool::RecognizeText(upscaleBuffer.GetView());

	return rawWaveText;
}
std::optional<int> WaveChangedBehavior::SanitizeWaveText(std::string rawWaveText) {
	std::string digitString = "";
	for (char c : rawWaveText) {
		if (std::isdigit(static_cast<unsigned char>(c)))
			digitString += c;
	}

	if (digitString.empty())
		return std::nullopt;

	return std::stol(digitString);
}
TickStatus WaveChangedBehavior::TickTbbf(TbbfMacroInstance* instance, TbbfCustomContext* context, const FrameView& currentFrame) {
	m_debounceMs = 5000;
	if (context->isBossPresent)
		return TickStatus::Skipped;

	if (context->playerStatus != PlayerStatus::Deployed)
		return TickStatus::Skipped;

	Rect waveTextBounds = context->uiLayout.waveTextBounds;
	FrameView croppedWaveText = PixelModifier::Crop(currentFrame, waveTextBounds.x, waveTextBounds.y, waveTextBounds.width, waveTextBounds.height);
	if (!croppedWaveText.data)
		return TickStatus::Skipped;

	if (!WaveTextExists(croppedWaveText))
		return TickStatus::Skipped;

	
	std::string rawWaveText = ParseWaveText(croppedWaveText);
	if (rawWaveText.find("Wave") == std::string::npos)
		return TickStatus::Skipped;

	std::optional<int> waveNumber = SanitizeWaveText(rawWaveText);
	if (!waveNumber || waveNumber > 39)
		return TickStatus::Skipped;

	if (context->waveNumber != *waveNumber) {
		context->waveNumber = *waveNumber;
		m_debounceMs = 30000;
		return TickStatus::Yield;
	}
	return TickStatus::Skipped;
}