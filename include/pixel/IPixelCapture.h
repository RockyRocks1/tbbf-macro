#pragma once
#include <windows.h>

struct Color {
	uint8_t r = 0, g = 0, b = 0, a 255;
};
class IPixelCapture {
public:
	virtual ~IPixelCapture() = default;
	virtual bool Initialize(HWND targetHwnd = nullptr) = 0;
	virtual bool CaptureRegion(int x, int y, int width, int height) = 0;

	virtual Color GetPixel(int relX, int relY) const = 0;
	virtual const uint8_t* GetBuffer() const = 0;
	virtual int GetWidth() const = 0;
	virtual int GetHeight() const = 0;
}