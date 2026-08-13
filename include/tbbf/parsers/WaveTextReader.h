#pragma once
#include <pixel/UDim2.h>
#include <pixel/FrameView.h>

class WaveTextReader {
public:
	static std::optional<int> ReadFromFrame(const FrameView& currentFrame, const Rect& waveTextBounds) noexcept;
private:
	static bool WaveTextExists(const FrameView& waveTextFrame) noexcept;
	static std::string ParseWaveText(const FrameView& waveTextFrame) noexcept;
	static std::optional<int> SanitizeWaveText(std::string rawWaveText) noexcept;
};