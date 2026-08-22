#pragma once
#include <pixel/UDim2.h>
#include <pixel/PixelAnalyzer.h>
#include <array>

class DisconnectReader {
public:
	static bool IsDisconnected(const FrameView& currentFrame) noexcept;
};