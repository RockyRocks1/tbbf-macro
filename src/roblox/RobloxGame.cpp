#include <roblox/RobloxGame.h>


RobloxGame::RobloxGame(HWND hwnd, PixelCaptureMode captureMode): m_hwnd(hwnd), m_captureMode(captureMode) {
	UpdateClientBounds();
};
std::unique_ptr<RobloxGame> RobloxGame::FromHwnd(HWND hwnd, PixelCaptureMode captureMode) {
	if (!WindowUtils::IsMainWindow(hwnd))
		return nullptr;
	std::unique_ptr<RobloxGame> game = std::unique_ptr<RobloxGame>(new RobloxGame(hwnd, captureMode));
	std::unique_ptr<IPixelCapture> capture;
	switch (captureMode) {
	case PixelCaptureMode::GDI:
		capture = std::make_unique<GdiPixelCapture>();
		break;
	case PixelCaptureMode::WGC:
		capture = std::make_unique<WgcPixelCapture>();
		break;
	default:
		return nullptr;
	};
	capture->Initialize(hwnd);
	game->m_pixelCapture = std::move(capture);
	return game;
}
std::unique_ptr<RobloxGame> RobloxGame::FromProcessId(DWORD processId, PixelCaptureMode captureMode) {
	HWND mainWindowHwnd = WindowUtils::FindMainWindow(processId);

	if (!mainWindowHwnd)
		return nullptr;

	return RobloxGame::FromHwnd(mainWindowHwnd, captureMode);
}
FrameView RobloxGame::GetLatestFrame() const {
	if (!m_pixelCapture)
		return {};
	if (!UpdateClientBounds())
		return {};
	FrameView fullFrame = m_pixelCapture->GetLatestFrame();
	if (!fullFrame.data)
		return {};
	
	std::optional<POINT> clientOffset;
	switch (m_captureMode) {
		case PixelCaptureMode::GDI:
			clientOffset = WindowUtils::GetClientOffsetFromWindow(m_hwnd);
			break;
		case PixelCaptureMode::WGC:
			clientOffset = WindowUtils::GetClientOffsetFromWgc(m_hwnd);
			break;
		default:
			break;
	}
	if (!clientOffset) 
		return {};

	return PixelModifier::Crop(fullFrame, clientOffset->x, clientOffset->y, m_clientBounds.width, m_clientBounds.height);
}
bool RobloxGame::UpdateClientBounds() const {
	if (!WinExists()) {
		m_clientBounds.width = 0;
		m_clientBounds.height = 0;
		return false;
	}
	RECT clientRect;
	if (!GetClientRect(m_hwnd, &clientRect))
		return false;

	m_clientBounds.width = clientRect.right;
	m_clientBounds.height = clientRect.bottom;
	return true;
}
