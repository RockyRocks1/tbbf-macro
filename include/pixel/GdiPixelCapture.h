#pragma once
#include <vector>
#include <wil/resource.h>
#include "IPixelCapture.h"

class GdiPixelCapture : public IPixelCapture {
private:
	HWND m_targetHwnd = nullptr;
	wil::unique_hbitmap m_hBitmap;
	wil::unique_hdc m_hdcMemory;
	BITMAPINFO m_bitmapInfo{};

	void* m_pBuffer = nullptr;
	int m_width = 0;
	int m_height = 0;
public:
	GdiPixelCapture() = default;
	~GdiPixelCapture() = default; // ?

	bool Initialize(HWND targetHwnd = nullptr) override;
	bool CaptureRegion(int x, int y, int width, int height) override;

	ColorRGBA GetPixel(int relX, int relY) const override;
	const uint8_t* GetBuffer() const override;
	int GetWidth() const override;
	int GetHeight() const override;
};
