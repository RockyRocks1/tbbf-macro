#include <pixel/GdiPixelCapture.h>



bool GdiPixelCapture::Initialize(HWND targetHwnd) {
	m_targetHwnd = targetHwnd;
	wil::unique_hdc_window hdcScreen = wil::GetDC(m_targetHwnd);
	if (!hdcScreen)
		return false;

	m_hdcMemory = wil::unique_hdc(CreateCompatibleDC(hdcScreen.get()));
	if (!m_hdcMemory)
		return false;

	BITMAPINFO bitmapInfo = {};
	bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitmapInfo.bmiHeader.biPlanes = 1;
	bitmapInfo.bmiHeader.biBitCount = 32;
	bitmapInfo.bmiHeader.biCompression = BI_RGB;
	m_bitmapInfo = bitmapInfo;

	return true;
}

bool GdiPixelCapture::CaptureRegion(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
	RECT captureBoundaries{};
	if (m_targetHwnd) {
		if (!GetClientRect(m_targetHwnd, &captureBoundaries))
			return false;
	} else {
		captureBoundaries.left = GetSystemMetrics(SM_XVIRTUALSCREEN);
		captureBoundaries.top = GetSystemMetrics(SM_YVIRTUALSCREEN);
		captureBoundaries.right = captureBoundaries.left + GetSystemMetrics(SM_CXVIRTUALSCREEN);
		captureBoundaries.bottom = captureBoundaries.top + GetSystemMetrics(SM_CYVIRTUALSCREEN);
	}
	if (x < captureBoundaries.left || y < captureBoundaries.top || (x + width) > captureBoundaries.right || (y + height) > captureBoundaries.bottom)
		return false;

	if (m_width != width || m_height != height || !m_hBitmap) {
		m_width = width;
		m_height = height;
		
		m_bitmapInfo.bmiHeader.biWidth = width;
		m_bitmapInfo.bmiHeader.biHeight = -height;
		m_hBitmap = wil::unique_hbitmap(CreateDIBSection(
			m_hdcMemory.get(),
			&m_bitmapInfo,
			DIB_RGB_COLORS,
			&m_pBuffer,
			nullptr,
			0
		));
		if (!m_hBitmap || !m_pBuffer) 
			return false;

		HBITMAP hOld = static_cast<HBITMAP>(SelectObject(m_hdcMemory.get(), m_hBitmap.get()));
		if (!m_hOldBitmap)
			m_hOldBitmap = hOld;
	}

	wil::unique_hdc_window hdcScreen = wil::GetDC(m_targetHwnd);
	if (!hdcScreen)
		return false;

	if (!BitBlt(m_hdcMemory.get(), 0, 0, width, height, hdcScreen.get(), x, y, SRCCOPY))
		return false;

	return true;
}

GdiPixelCapture::~GdiPixelCapture() {
	if (m_hdcMemory && m_hOldBitmap) {
		SelectObject(m_hdcMemory.get(), m_hOldBitmap);
		m_hOldBitmap = nullptr;
	}
	m_hBitmap.reset();
	m_hdcMemory.reset();
}