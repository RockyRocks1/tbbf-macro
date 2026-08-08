#include <tbbf/TbbfMacroInstance.h>
static inline uint64_t GetCurrentTimestampMs() {
	using namespace std::chrono;
	return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}
void TbbfMacroInstance::PerformMainLoop() {
	while (m_isLoopRunning.load()) {
		uint64_t currentTimestampMs = GetCurrentTimestampMs();
		if (!m_game || !m_game->WinExists()) {
			m_isLoopRunning.store(false);
			continue;
		}
		const FrameView latestFrame = m_game->GetLatestFrame();
		if (!latestFrame.data) {
			std::this_thread::sleep_for(std::chrono::milliseconds(16));
			continue;
		}

		for (const auto& behavior : m_behaviors) {
			if (!behavior->CanTick(currentTimestampMs))
				continue;

			TickStatus status = behavior->Tick(this, m_context.get(), latestFrame);
			behavior->UpdateLastTickTime(currentTimestampMs);
			if (status == TickStatus::Terminated)
				m_isLoopRunning.store(false);
			if (status != TickStatus::Skipped) {
				break;
			}
				
			
		}
		
		std::this_thread::sleep_for(std::chrono::milliseconds(16));
	}
}

bool TbbfMacroInstance::Initialize(std::unique_ptr<RobloxGame> game) {
	if (!game)
		return false;
	m_game = std::move(game);
	m_context = std::make_unique<TbbfCustomContext>();
	// ORDER:
	// CRITICAL
	m_behaviors.push_back(std::make_unique<DisconnectBehavior>());
	m_behaviors.push_back(std::make_unique<GameLoadedBehavior>());
	m_behaviors.push_back(std::make_unique<CentralContextBehavior>());
	// HIGH
	m_behaviors.push_back(std::make_unique<VoteMenuBehavior>());
	m_behaviors.push_back(std::make_unique<TowerListBehavior>());
	// MEDIUM

	// LOW	


	m_isLoopRunning.store(true);
	m_thread = std::jthread([this]() {
		this->PerformMainLoop();
	});
	return true;
}
void TbbfMacroInstance::SendKey(WORD virtualKey) const {
	m_game->SetFocus();
	INPUT input{};
	input.type = INPUT_KEYBOARD;
	input.ki.wVk = 0;
	input.ki.time = 0;
	input.ki.dwExtraInfo = 0;
	input.ki.wScan = MapVirtualKey(virtualKey, MAPVK_VK_TO_VSC);

	DWORD flags = KEYEVENTF_SCANCODE;
	if (virtualKey == VK_DOWN || virtualKey == VK_RIGHT ||
		virtualKey == VK_LEFT || virtualKey == VK_UP) {
		flags |= KEYEVENTF_EXTENDEDKEY;
	}
	input.ki.dwFlags = flags;
	SendInput(1, &input, sizeof(INPUT));
	input.ki.dwFlags |= KEYEVENTF_KEYUP;
	SendInput(1, &input, sizeof(INPUT));

}
void TbbfMacroInstance::RepeatKey(WORD virtualKey, int repetitions) const {
	for (int i = 0; i < repetitions; i++)
		SendKey(virtualKey);
}

void TbbfMacroInstance::ToggleUiFocus() const {
	SendKey(VK_OEM_5); // Backslash
}

void TbbfMacroInstance::ClickClient(const POINT& clickPosition) const {
	FrameView currentFrame = m_game->GetLatestFrame();
	if (clickPosition.x < 0 || clickPosition.x > currentFrame.width || clickPosition.y < 0 || clickPosition.y > currentFrame.height)
		return;
	POINT screenPoint = clickPosition;
	ClientToScreen(m_game->GetHwnd(), &screenPoint);
	std::optional<POINT> absolutePoint = WindowUtils::ToAbsoluteCoordinates(screenPoint);

	if (!absolutePoint)
		return;

	m_game->SetFocus();
	INPUT input{};
	input.type = INPUT_MOUSE;
	input.mi.dx = absolutePoint->x;
	input.mi.dy = absolutePoint->y;
	input.mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;
	SendInput(1, &input, sizeof(INPUT));

	input.type = INPUT_MOUSE;
	input.mi.dx = 1;
	input.mi.dy = 0;
	input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_MOVE;
	SendInput(1, &input, sizeof(INPUT));

	input.type = INPUT_MOUSE;
	input.mi.dx = 0;
	input.mi.dy = 0;
	input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
	SendInput(1, &input, sizeof(INPUT));
}