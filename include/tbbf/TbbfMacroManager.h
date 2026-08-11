#pragma once
#include <macro/IMacroManager.h>
#include <roblox/RobloxWeb.h>
#include <roblox/RobloxLauncher.h>
#include "TbbfMacroInstance.h"
#include <thread>

class TbbfMacroManager : IMacroManager {
private:
	bool m_isRunning = false;
	std::mutex m_instancesMutex;
	std::vector<std::unique_ptr<IMacroInstance>> m_instances;
	std::vector<std::jthread> m_launchThreads;

	void LinkMacroInstance(HWND hwnd, const MacroInstanceLaunchInfo& launchInfo);
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