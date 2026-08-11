#pragma once
#include <macro/IMacroInstance.h>
#include <macro/IContext.h>


enum class SplashTextStatus {
	Unknown = 0,
	Vote = 1,
	Welcome = 2,
};
enum class VoteStatus {
	Loading = 0,
	MapVote = 1,
	GamemodeVote = 2,
	Waiting = 3,
	Finished = 4
};
enum class TowerSelectStatus {
	Inactive = 0,
	Menuing = 1,
	Selecting = 2,
	Buying = 3,
	Equipping = 4,
};
enum class PlayerStatus {
	Loading = 0,
	Menu = 1,
	Deployed = 2,
	Dead = 3,
	Respawning = 4,
};
enum class TowerListStatus {
	Uninitialized = 0,
	Reading = 1,
	Done = 2,
};
enum class TowerSelection {
	Scout = 0,
	Enforcer = 1,
	VoidTraitor = 2,
};
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
};
struct TowerListInfo {
	TowerListStatus status = TowerListStatus::Uninitialized;
	int currentReadIndex = 0;
	int enforcerIndex = -1;
	int voidTraitorIndex = -1;
};
struct TbbfCustomContext : public IContext {
	UiLayoutInfo uiLayout{};
	TowerListInfo towerListInfo{};
	POINT spawnPosition{400, 400};
	TowerSelectStatus towerSelectStatus = TowerSelectStatus::Inactive;
	SplashTextStatus splashStatus = SplashTextStatus::Unknown;
	PlayerStatus playerStatus = PlayerStatus::Loading;
	VoteStatus voteStatus = VoteStatus::Loading;
	int waveNumber = 0;
	int lastProcessedWave = -1;
	uint64_t lastRespawnTick = 0;
	bool isBossPresent = false;
	bool toolEquipped = false;
	bool inGameMenuOpen = false;
};