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
	timeBeginPeriod(1);
	HANDLE currentThreadHandle = ::GetCurrentThread();
	::SetThreadPriority(currentThreadHandle, THREAD_PRIORITY_TIME_CRITICAL);
	while (m_isRunning.load()) {
		std::vector<TbbfMacroInstance*> activeInstances;
		std::vector<size_t> deadInstancesIndices;
		{
			std::lock_guard<std::mutex> lock(m_instancesMutex);
			for (size_t i = 0; i < m_instances.size(); ++i) {
				if (!m_instances[i].instance)
					continue;

				TbbfMacroInstance* tbbfInstance = static_cast<TbbfMacroInstance*>(m_instances[i].instance.get());
				if (!tbbfInstance)
					continue;

				if (!tbbfInstance->IsRunning()) {
					deadInstancesIndices.push_back(i);
					LaunchMacroInstance(m_instances[i].launchInfo);
					continue;
				}				
				activeInstances.push_back(tbbfInstance);
			}

			for (auto it = deadInstancesIndices.rbegin(); it != deadInstancesIndices.rend(); ++it) {
				m_instances.erase(m_instances.begin() + *it);
			}
		}

		if (activeInstances.empty() && deadInstancesIndices.empty()) {
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
			continue;
		}
		auto frameStart = std::chrono::steady_clock::now();
		for (TbbfMacroInstance* instance : activeInstances) {
			if (!m_isRunning.load())
				break;
			instance->Tick();
		}
		auto frameEnd = std::chrono::steady_clock::now();

		auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(frameEnd - frameStart);

		/*if (elapsedTime < std::chrono::milliseconds(16) && m_isRunning.load())
			std::this_thread::sleep_for(std::chrono::milliseconds(16) - elapsedTime);*/
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	timeEndPeriod(1);
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
		m_instances.push_back(MacroInstanceWrapper{
			std::move(instance),
			launchInfo 
		});
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
	if (instanceIndex < 0 || instanceIndex >= static_cast<int>(m_instances.size())) 
		return false;
	

	m_instances.erase(m_instances.begin() + instanceIndex);
	return true;
}
