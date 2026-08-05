#include <pixel/GdiPixelCapture.h>



bool GdiPixelCapture::Initialize(HWND targetHwnd) {
	if (m_status.load() != PixelCaptureStatus::Uninitialized)
		return false;

	m_targetHwnd = targetHwnd;
	wil::unique_hdc_window hdcScreen = wil::GetDC(nullptr);
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

	m_status.store(PixelCaptureStatus::Running);
	return true;
}
void GdiPixelCapture::Close() {
	m_status.store(PixelCaptureStatus::Closed);
	if (m_hdcMemory && m_hOldBitmap) {
		SelectObject(m_hdcMemory.get(), m_hOldBitmap);
		m_hOldBitmap = nullptr;
	}
	m_hBitmap.reset();
	m_hdcMemory.reset();
}
bool GdiPixelCapture::CaptureRegion(const Rect& region) {
	RECT windowRect{};
	if (m_status.load() != PixelCaptureStatus::Running)
		return false;
	if (!GetWindowRect(m_targetHwnd, &windowRect))
		return false;
	if (region.width <= 0 || region.height <= 0)
		return false;

	if (region.x < 0 || region.y < 0 || (region.x + region.width) > windowRect.right || (region.y + region.height) > windowRect.bottom)
		return false;

	if (m_width != region.width || m_height != region.height || !m_hBitmap) {
		m_width = region.width;
		m_height = region.height;
		
		m_bitmapInfo.bmiHeader.biWidth = region.width;
		m_bitmapInfo.bmiHeader.biHeight = -region.height;
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

	if (!BitBlt(m_hdcMemory.get(), 0, 0, region.width, region.height, hdcScreen.get(), region.x, region.y, SRCCOPY))
		return false;

	return true;
}
bool GdiPixelCapture::CaptureClientRegion(const Rect& clientRegion) {
	if (clientRegion.width <= 0 || clientRegion.height <= 0 || clientRegion.x < 0 || clientRegion.y < 0)
		return false;

	std::optional<POINT> clientOffect = WindowUtils::GetClientOffsetFromWindow(m_targetHwnd);

	if (!clientOffect)
		return false;
	
	Rect windowRegion{
		clientRegion.x + clientOffect->x,
		clientRegion.y + clientOffect->y,
		clientRegion.width,
		clientRegion.height
	};
	return CaptureRegion(windowRegion);
}
GdiPixelCapture::~GdiPixelCapture() {
	Close();
}