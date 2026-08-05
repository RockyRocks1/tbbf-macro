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
GdiPixelCapture::~GdiPixelCapture() {
	Close();
}