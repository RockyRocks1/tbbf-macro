#pragma once
#include <vector>
#include <wil/resource.h>
#include <atomic>
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
	std::atomic<PixelCaptureStatus> m_status{ PixelCaptureStatus::Uninitialized };
public:
	GdiPixelCapture() = default;
	~GdiPixelCapture() override;

	bool Initialize(HWND targetHwnd = nullptr) override;
	void Close() override;
	inline PixelCaptureStatus GetStatus() const {
		return m_status.load();
	}
	inline FrameView GetLatestFrame() const {
		return {
			.data = nullptr,
			.width = m_width,
			.height = m_height,
			.stride = static_cast<size_t>(m_width) * 4, // temp
			.format = PixelFormat::Bgra8
		};
	}
};
