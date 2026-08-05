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
bool RobloxGame::UpdateClientBounds() {
	if (!WinExists()) {
		m_clientBounds = Rect{};
		return false;
	}
	RECT clientRect;
	if (!GetClientRect(m_hwnd, &clientRect))
		return false;
	m_clientBounds = {
		0,
		0,
		clientRect.right - clientRect.left,
		clientRect.bottom - clientRect.top
	};
	return true;
}
bool RobloxGame::WasDisconnected() const {
	if (!WinExists())
		return false;
	const FrameView frameView = m_pixelCapture->GetFrameView();
	if (!frameView.data)
		return false;

	static constexpr std::array<UDim2, 4> disconnectUDims = {
		UDim2(0.5, -198, 0.5, -124),
		UDim2(0.5, 198, 0.5, -124),
		UDim2(0.5, -198, 0.5, 124),
		UDim2(0.5, 198, 0.5, 124)
	};
	static constexpr ColorRgba targetGrayColor{ 0x39, 0x3B, 0x3D };
	
	for (UDim2 udim2 : disconnectUDims) {
		POINT pixelPosition = udim2.Resolve(m_clientBounds);
		std::optional<ColorRgba> pixelColor = PixelAnalyzer::GetPixelColor(frameView, pixelPosition);
		if (!pixelColor || *pixelColor != targetGrayColor)
			return false;
	}
	return true;
}
