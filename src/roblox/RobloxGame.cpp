#include <roblox/RobloxGame.h>


RobloxGame::RobloxGame(HWND hwnd) : m_hwnd(hwnd) {
	UpdateClientBounds();
};
std::optional<RobloxGame> RobloxGame::FromHwnd(HWND hwnd, PixelCaptureMode captureMode) {
	if (!WindowUtils::IsMainWindow(hwnd))
		return std::nullopt;
	RobloxGame game(hwnd);
	std::unique_ptr<IPixelCapture> capture;
	switch (captureMode) {
	case PixelCaptureMode::GDI:
		capture = std::make_unique<GdiPixelCapture>();
		break;
	case PixelCaptureMode::WGC:
		capture = std::make_unique<WgcPixelCapture>();
		break;
	default:
		return std::nullopt;
	};
	capture->Initialize(hwnd);
	game.m_pixelCapture = std::move(capture);
	return game;
}
std::optional<RobloxGame> RobloxGame::FromProcessId(DWORD processId, PixelCaptureMode captureMode) {
	HWND mainWindowHwnd = WindowUtils::FindMainWindow(processId);

	if (!mainWindowHwnd)
		return std::nullopt;

	return RobloxGame::FromHwnd(mainWindowHwnd, captureMode);
}
void RobloxGame::UpdateClientBounds() {

}
