#include <roblox/RobloxGame.h>


RobloxGame::RobloxGame(HWND hwnd, PixelCaptureMode captureMode): m_hwnd(hwnd), m_captureMode(captureMode) {
	UpdateClientBounds();
};
std::optional<RobloxGame> RobloxGame::FromHwnd(HWND hwnd, PixelCaptureMode captureMode) {
	if (!WindowUtils::IsMainWindow(hwnd))
		return std::nullopt;
	RobloxGame game(hwnd, captureMode);
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
// WARNING: this requires setting roblox menu background opacity to zero
bool RobloxGame::WasGameLoaded(const FrameView& currentFrame) const {
	if (!WinExists())
		return false;
	if (!currentFrame.data)
		return false;

	static constexpr std::array<UDim2, 3> gameLoadedUDims = {
		UDim2(0, 38, 0, 32),
		UDim2(0, 20, 0, 32),
		UDim2(0, 56, 0, 32)
	};
	const int maxVariation = 1;
	static constexpr ColorRgba targetDefaultColor{ 0x12, 0x12, 0x15 };
	for (const UDim2& udim2 : gameLoadedUDims) {
		POINT pixelPosition = udim2.Resolve(m_clientBounds);
		std::optional<ColorRgba> pixelColor = PixelAnalyzer::GetPixelColor(currentFrame, pixelPosition);
		if (!pixelColor || !targetDefaultColor.IsCloseTo(*pixelColor, maxVariation))
			return false;
	}
	return true;
}

bool RobloxGame::WasDisconnected(const FrameView& currentFrame) const {
	if (!WinExists())
		return false;
	if (!currentFrame.data)
		return false;

	static constexpr std::array<UDim2, 4> disconnectUDims = {
		UDim2(0.5, -199, 0.5, -124),
		UDim2(0.5, 199, 0.5, -124),
		UDim2(0.5, -199, 0.5, 124),
		UDim2(0.5, 199, 0.5, 124)
	};
	const int maxVariation = 1;
	static constexpr ColorRgba targetGrayColor{ 0x39, 0x3B, 0x3D };
	for (const UDim2& udim2 : disconnectUDims) {
		POINT pixelPosition = udim2.Resolve(m_clientBounds);
		std::optional<ColorRgba> pixelColor = PixelAnalyzer::GetPixelColor(currentFrame, pixelPosition);
		if (!pixelColor || !targetGrayColor.IsCloseTo(*pixelColor, maxVariation))
			return false;
	}
	return true;
}
