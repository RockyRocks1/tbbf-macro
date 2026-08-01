#pragma once
#include <vector>
#include <wil/resource.h>
#include "IPixelCapture.h"

class GdiPixelCapture : public IPixelCapture {
private:
	HWND m_targetHwnd = nullptr;
	wil::unique_hdc m_hdcScreen;
	wil::unique_hdc m_hdcMemory;
	wil::unique_hbitmap m_hBitmap;

	HBITMAP m_oldBitmap = nullptr;

	std::vector<uint8_t> m_buffer;
	int m_width = 0;
	int m_height = 0;
public:
	GdiPixelCapture() = default;
	~GdiPixelCapture() override;

	bool Initialize(HWND targetHwnd = nullptr) override;
	bool CaptureRegion(int x, int y, int width, int height) override;

	Color GetPixel(int relX, int relY) const override;
	const uint8_t* GetBuffer() const override;
	int GetWidth() const override;
	int GetHeight() const override;
};
