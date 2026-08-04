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
	HBITMAP m_hOldBitmap;

	void* m_pBuffer = nullptr;
	uint32_t m_width = 0;
	uint32_t m_height = 0;
	Rect GetClientBounds() const override;
public:
	GdiPixelCapture() = default;
	~GdiPixelCapture() override;

	bool Initialize(HWND targetHwnd = nullptr) override;
	bool CaptureRegion(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

	ColorRgba GetPixel(uint32_t relX, uint32_t relY) const override;

	inline FrameView GetFrameView() const noexcept {
		return {
			.data = static_cast<const uint8_t*>(m_pBuffer),
			.width = m_width,
			.height = m_height,
			.stride = m_width * 4, // temp
			.format = PixelFormat::Bgra8
		};
	}
	inline uint32_t GetWidth() const noexcept override {
		return m_width;
	}
	inline uint32_t GetHeight() const noexcept override {
		return m_height;
	}
};
