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
enum class PlayerStatus {
	Loading = 0,
	Menu = 1,
	Buying = 2,
	Deployed = 3,
	Dead = 4,
};
enum class TowerListStatus {
	Uninitialized = 0,
	Reading = 1,
	Done = 2,
};
struct UiLayoutInfo {
	Size2D viewportSize{ 0, 0 };
	POINT inGameMenuAnchor{ 0, 0 };
	Rect healthBarBounds{ 0, 0, 0, 0 };
	Rect bossHealthBarBounds{ 0, 0, 0, 0 };
	Rect waveTextBounds{ 0, 0, 0, 0 };
	bool HasResized(const FrameView& frame) const {
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
	POINT spawnPosition{};
	SplashTextStatus splashStatus = SplashTextStatus::Unknown;
	PlayerStatus playerStatus = PlayerStatus::Loading;
	VoteStatus voteStatus = VoteStatus::Loading;
	bool isBossPresent = false;
};