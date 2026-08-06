#include <tbbf/TbbfMacroInstance.h>

void TbbfMacroInstance::PerformMainLoop() {
	while (m_isLoopRunning.load()) {
		if (!m_game || !m_game->WinExists())
			break;
		const FrameView latestFrame = m_game->GetLatestFrame();
		if (!latestFrame.data) {
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			continue;
		}
		TickResult finalResult{};
		for (const auto& behavior : m_behaviors) {
			finalResult = behavior->OnTick(this, latestFrame);
			if (finalResult.status != TickStatus::Skipped)
				break;
		}
		if (finalResult.status == TickStatus::Terminated) {
			m_isLoopRunning.store(false);
			break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(finalResult.waitTimeMs));
	}
}

bool TbbfMacroInstance::Initialize(std::unique_ptr<RobloxGame> game) {
	if (!game)
		return false;
	m_game = std::move(game);
	// ORDER MATTERS:
	// CRITICAL
	m_behaviors.push_back(std::make_unique<DisconnectBehavior>());
	// HIGH
	m_behaviors.push_back(std::make_unique<GameLoadedBehavior>());
	// MEDIUM

	// LOW	
	m_isLoopRunning.store(true);
	m_thread = std::jthread([this]() {
		this->PerformMainLoop();
	});
	return true;
}