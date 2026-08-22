#pragma once
#include <macro/IMacroManager.h>
#include <roblox/RobloxWeb.h>
#include <roblox/RobloxLauncher.h>
#include "TbbfMacroInstance.h"
#include <thread>

class TbbfMacroManager : IMacroManager {
private:
	struct MacroInstanceWrapper {
		std::unique_ptr<IMacroInstance> instance;
		MacroInstanceLaunchInfo launchInfo;
	};
	std::atomic<bool> m_isRunning{ false };
	std::mutex m_instancesMutex;
	std::vector<MacroInstanceWrapper> m_instances;
	std::vector<std::jthread> m_launchThreads;
	std::jthread m_managerThread;

	void PerformManagerLoop();
	void CreateMacroInstance(const MacroInstanceLaunchInfo& launchInfo);
	void CreateMacroInstance(HWND hwnd, const MacroInstanceLaunchInfo& launchInfo);
public:
	bool Start() override;
	void Stop() override;

	bool LaunchMacroInstance(const MacroInstanceLaunchInfo& launchInfo) override;
	bool LaunchMacroInstance(HWND hwnd, const MacroInstanceLaunchInfo& launchInfo) override;
	bool TerminateMacroInstance(int instanceIndex) override;

	inline size_t GetActiveInstanceCount() const {
		return 1;
	}
	inline bool IsRunning() const {
		return true;
	}
};