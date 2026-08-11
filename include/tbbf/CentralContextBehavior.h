#pragma once
#include "TbbfMacroBehavior.h"
#include "TbbfCustomContext.h"
#include <pixel/UDim2.h>

class TbbfMacroInstance;

class CentralContextBehavior : public TbbfMacroBehavior {
private:
	Rect GetHealthBarBounds(const FrameView& currentFrame);
	POINT GetInGameMenuAnchor(const FrameView& currentFrame);
	Rect GetWaveTextBounds(const FrameView& currentFrame, POINT& inGameMenuAnchor);
	Rect GetExpBarBounds(const FrameView& currentFrame, POINT& inGameMenuAnchor);

	SplashTextStatus GetSplashTextStatus(TbbfMacroInstance* instance, const FrameView& currentFrame);
	PlayerStatus GetPlayerStatus(TbbfMacroInstance* instance, TbbfCustomContext* context, const FrameView& currentFrame);
	
	bool IsDead(const FrameView& currentFrame, const Rect& healthBarBounds);
	bool IsDeployed(const FrameView& currentFrame, const Rect& expBarBounds);
	bool HasInvincibility(uint64_t currentTick, uint64_t lastRespawnTick);
protected:
	TickStatus TickTbbf(TbbfMacroInstance* instance, TbbfCustomContext* context, const FrameView& currentFrame) override;
public:
	CentralContextBehavior() : TbbfMacroBehavior(16) {};
};
