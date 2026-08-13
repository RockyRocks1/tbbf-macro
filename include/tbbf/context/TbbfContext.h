#pragma once
#include <macro/IContext.h>
#include <macro/IMacroInstance.h>
#include "MacroTask.h"
#include <unordered_map>

enum class TowerListStatus {
	Pending,
	Ready
};
enum class SplashTextStatus {
	Unknown,
	Vote, 
	Welcome
};
enum class PlayerStatus {
	Loading,
	Menu,
	Deployed,
	Dead
};
enum class TargetGamemode { 
	Classic, 
	Hardmode
};
struct TbbfContext : public IContext {
	POINT spawnPosition{ 400, 400 };
	struct TbbfMacroDecisions {
		TbbfMacroTask currentTask = TbbfMacroTask::Idle;
		TbbfMacroTask lastExecutedTask = TbbfMacroTask::Idle;
		bool shouldAttack = false;
		bool shouldUpgrade = false;
		bool shouldEquipTool = false;
		bool shouldOpenMenu = false;
	} decisions{};

	struct UiLayoutInfo {
		Size2D viewportSize{ 0, 0 };
		POINT inGameMenuAnchor{ 0, 0 };
		int inGameMenuOpenOffset = 0;
		Rect healthBarBounds{ 0, 0, 0, 0 };
		Rect expBarBounds{ 0, 0, 0, 0 };
		Rect bossHealthBarBounds{ 0, 0, 0, 0 };
		Rect waveTextBounds{ 0, 0, 0, 0 };
		bool HasResized(const FrameView& frame) const noexcept {
			return viewportSize.width != frame.width || viewportSize.height != frame.height;
		}
	} uiLayout{};

	struct TowerListInfo {
		TowerListStatus status = TowerListStatus::Pending;
		std::unordered_map<std::string, int> registry{};
		int currentReadIndex = 0;
		std::optional<int> GetTowerIndex(const std::string& towerName) const noexcept {
			auto it = registry.find(towerName);
			if (it != registry.end())
				return it->second;
			return std::nullopt;
		}
	} towerList{};

	struct TimestampInfo {
		uint64_t lastRespawnTick = 0;
		uint64_t lastWaveChangedTick = 0;
		uint64_t nextOcrAllowedTick = 0;
		uint64_t nextVoteActionTick = 0;
	} timestamps{};

	struct ToolInfo {
		bool exists = false;
		bool isActive = false;
		uint64_t lastEquippedTick = 0;
	} tool{};

	PlayerStatus playerStatus = PlayerStatus::Loading;
	SplashTextStatus splashStatus = SplashTextStatus::Unknown;

	int waveNumber = 0;
	bool isDisconnected = false;
	bool isBossPresent = false;
	bool isMiniMenuActive = false;
};
