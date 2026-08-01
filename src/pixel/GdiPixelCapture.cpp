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

bool GdiPixelCapture::CaptureRegion(int x, int y, int width, int height) {
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
	if (x + width > captureBoundaries.right || y + height > captureBoundaries.bottom)
		return false;

	wil::unique_hdc_window hdcScreen = wil::GetDC(m_targetHwnd);
	if (!hdcScreen) 
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
		SelectObject(m_hdcMemory.get(), m_hBitmap.get());
	}

	if (!BitBlt(m_hdcMemory.get(), 0, 0, width, height, hdcScreen.get(), x, y, SRCCOPY))
		return false;

	return true;
}

ColorRGBA GdiPixelCapture::GetPixel(int relX, int relY) const {
	if (!m_pBuffer)
		return ColorRGBA{ 0, 0, 0, 0 };
	if (relX >= m_width || relY >= m_height || relX < 0 || relY < 0)
		return ColorRGBA{ 0, 0, 0, 0 };

	const size_t index = static_cast<size_t>(relY * m_width + relX);

	const ColorBGRA* pixelBytes = static_cast<const ColorBGRA*>(m_pBuffer);
	const ColorBGRA pixel = pixelBytes[index];

	return ColorRGBA{ pixel.r, pixel.g, pixel.b, pixel.a};
}

const uint8_t* GdiPixelCapture::GetBuffer() const {
	return static_cast<const uint8_t*>(m_pBuffer);
}

int GdiPixelCapture::GetWidth() const {
	return m_width;
}
int GdiPixelCapture::GetHeight() const {
	return m_height;
}