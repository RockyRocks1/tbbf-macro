#pragma once
#include <macro/IContext.h>
#include <macro/IMacroInstance.h>
#include <unordered_map>

enum class TowerListStatus {
	Idle,
	Reading,
	Ready
};
enum class TowerSelectStatus {
	Idle,
	Selecting,
	Buying,
	Equipping
};
enum class VoteMenuStatus {
	Idle,
	VotingMap,
	VotingGamemode
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
	struct Decisions {
		bool commandShutdown = false;
		bool commandVoteMenu = false;
		bool commandGoToMenu = false;
		bool commandReadTowerList = false;
		bool commandSelectTower = false;

		bool shouldAttack = false;
		bool shouldUpgrade = false;
		bool shouldEquipTool = false;
		bool shouldOpenMenu = false;
		bool shouldDeploy = false;

		bool isWaitingForMenu = false;
	} decisions{};

	struct UiLayoutInfo {
		Size2D viewportSize{ 0, 0 };
		POINT inGameMenuAnchor{ 0, 0 };
		int inGameMenuOpenOffset = 0;
		Rect healthBarBounds{ 0, 0, 0, 0 };
		Rect expBarBounds{ 0, 0, 0, 0 };
		Rect bossHealthBarBounds{ 0, 0, 0, 0 };
		Rect waveTextBounds{ 0, 0, 0, 0 };
		std::optional<POINT> skipVoteButtonPos = std::nullopt;
		bool HasResized(const FrameView& frame) const noexcept {
			return viewportSize.width != frame.width || viewportSize.height != frame.height;
		}
	} uiLayout{};
	struct BehaviorStatuses {
		TowerListStatus towerList = TowerListStatus::Idle;
		TowerSelectStatus towerSelect = TowerSelectStatus::Idle;
		VoteMenuStatus voteMenu = VoteMenuStatus::Idle;
	} statuses;
	struct TowerListInfo {
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
		uint64_t lastWaveReadTick = 0;
		uint64_t lastWaveChangedTick = 0;
	} timestamps{};

	struct ToolInfo {
		bool exists = false;
		bool isActive = false;
	} tool{};

	PlayerStatus playerStatus = PlayerStatus::Loading;
	SplashTextStatus splashStatus = SplashTextStatus::Unknown;

	int waveNumber = -1;
	int lastProcessedWave = -1;
	bool isInvincible = false;
	bool isDisconnected = false;
	bool isGameLoaded = false;
	bool isBossPresent = false;
	bool isMiniMenuActive = false;
};
