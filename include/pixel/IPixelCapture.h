#pragma once

#include <windows.h>
#include "FrameView.h"

enum class PixelCaptureMode {
	GDI = 0,
	WGC = 1
};
struct Rect {
	int x = 0;
	int y = 0;
	int width = 0;
	int height = 0;
};

class IPixelCapture {
public:
	virtual ~IPixelCapture() = default;
	virtual bool Initialize(HWND targetHwnd = nullptr) = 0;
	virtual bool CaptureRegion(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;

	virtual ColorRgba GetPixel(uint32_t relX, uint32_t relY) const = 0;
	virtual FrameView GetFrameView() const = 0;
	virtual uint32_t GetWidth() const = 0;
	virtual uint32_t GetHeight() const = 0;
private:
	virtual Rect GetClientBounds() const = 0;
};
