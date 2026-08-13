#include <tbbf/TbbfMacroManager.h>

bool TbbfMacroManager::Start() {
	if (m_isRunning.load())
		return true;
	m_isRunning.store(true);
	
	m_managerThread = std::jthread([this]() {
		this->PerformManagerLoop();
	});

	return true;
}
void TbbfMacroManager::Stop() {
	m_isRunning.store(false);
}

void TbbfMacroManager::PerformManagerLoop() {
	while (m_isRunning.load()) {
		std::vector<TbbfMacroInstance*> activeInstances;
		{
			std::lock_guard<std::mutex> lock(m_instancesMutex);
			for (const auto& instance : m_instances) {
				if (!instance)
					continue;

				TbbfMacroInstance* tbbfInstance = static_cast<TbbfMacroInstance*>(instance.get());
				if (tbbfInstance && tbbfInstance->IsRunning()) {
					activeInstances.push_back(tbbfInstance);
				}
			}
		}

		if (activeInstances.empty()) {
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
			continue;
		}

		for (TbbfMacroInstance* instance : activeInstances) {
			if (!m_isRunning.load())
				break;
			instance->Tick();
			std::this_thread::sleep_for(std::chrono::milliseconds(16));
		}
	}
}


void TbbfMacroManager::CreateMacroInstance(HWND hwnd, const MacroInstanceLaunchInfo& launchInfo) {
	std::unique_ptr<RobloxGame> game = RobloxGame::FromHwnd(hwnd);
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
void TbbfMacroManager::CreateMacroInstance(const MacroInstanceLaunchInfo& launchInfo) {
	RobloxWeb web(launchInfo.robloSecurity);
	std::optional<std::string> authTicket = web.GetAuthenticationTicket();
	if (!authTicket)
		return;
	
	RobloxLauncher launcher(launchInfo.gamePath);
	std::optional<DWORD> robloxProcessId = launcher.JoinPrivateGame(launchInfo.gameId, launchInfo.privateServerAccessCode, *authTicket);
	if (!robloxProcessId)
		return;
	HWND robloxHwnd{};
	while (!robloxHwnd) {
		robloxHwnd = WindowUtils::FindMainWindow(*robloxProcessId);
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	CreateMacroInstance(robloxHwnd, launchInfo);
}
bool TbbfMacroManager::LaunchMacroInstance(const MacroInstanceLaunchInfo& launchInfo) {
	m_launchThreads.emplace_back([this, launchInfo]() {
		this->CreateMacroInstance(launchInfo);
	});

	return true;
}
bool TbbfMacroManager::LaunchMacroInstance(HWND hwnd, const MacroInstanceLaunchInfo& launchInfo) {
	m_launchThreads.emplace_back([this, hwnd, launchInfo]() {
		this->CreateMacroInstance(hwnd, launchInfo);
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
