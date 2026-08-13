#pragma once
#include <pixel/UDim2.h>
#include <pixel/FrameView.h>

class DisconnectReader {
public:
	static bool IsDisconnected(const FrameView& currentFrame) noexcept;
};