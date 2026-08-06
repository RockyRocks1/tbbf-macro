#include <tbbf/TbbfMacroManager.h>

bool TbbfMacroManager::Start() {
	m_isRunning = true;
	return true;
}

void TbbfMacroManager::Stop() {
	m_isRunning = false;
}
void TbbfMacroManager::CreateMacroInstance(const MacroInstanceLaunchInfo& launchInfo) {
	RobloxWeb web(launchInfo.robloSecurity);
	std::optional<std::string> authTicket = web.GetAuthenticationTicket();
	if (!authTicket)
		return;
	
	RobloxLauncher launcher(launchInfo.gamePath);
	std::optional<DWORD> robloxProcessId = launcher.JoinPrivateGame(launchInfo.gameId, launchInfo.privateServerAccessCode, *authTicket);
	if (!robloxProcessId)
		return;
	while (!WindowUtils::FindMainWindow(*robloxProcessId))
		std::this_thread::sleep_for(std::chrono::seconds(1));
	std::unique_ptr<RobloxGame> game = RobloxGame::FromProcessId(*robloxProcessId);
	if (!game)
		return;

	
	std::unique_ptr<IMacroInstance> instance = std::make_unique<TbbfMacroInstance>();

	if (!instance->Initialize(std::move(game)))
		return;
	{
		std::lock_guard<std::mutex> lock(m_instancesMutex);
		m_instances.push_back(std::move(instance));
	}
}
bool TbbfMacroManager::LaunchMacroInstance(const MacroInstanceLaunchInfo& launchInfo) {
	m_launchThreads.emplace_back([this, launchInfo]() {
		this->CreateMacroInstance(launchInfo);
	});

	return true;
}

bool TbbfMacroManager::TerminateMacroInstance(int instanceIndex) {
	std::lock_guard<std::mutex> lock(m_instancesMutex);
	if (!m_instances.at(instanceIndex))
		return false;
	
	m_instances.erase(m_instances.begin() + instanceIndex);
	return true;
}
