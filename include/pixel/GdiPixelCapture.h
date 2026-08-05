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
	HBITMAP m_hOldBitmap{};

	void* m_pBuffer = nullptr;
	int m_width = 0;
	int m_height = 0;
public:
	GdiPixelCapture() = default;
	~GdiPixelCapture() override;

	bool Initialize(HWND targetHwnd = nullptr) override;
	bool CaptureRegion(const Rect& region) override;
	bool CaptureClientRegion(const Rect& clientRegion) override;

	inline FrameView GetFrameView() const noexcept {
		return {
			.data = static_cast<const uint8_t*>(m_pBuffer),
			.width = m_width,
			.height = m_height,
			.stride = static_cast<size_t>(m_width) * 4, // temp
			.format = PixelFormat::Bgra8
		};
	}
	inline int GetWidth() const noexcept override {
		return m_width;
	}
	inline int GetHeight() const noexcept override {
		return m_height;
	}
};
