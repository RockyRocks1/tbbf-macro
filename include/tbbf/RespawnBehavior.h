#pragma once
#include "TbbfMacroBehavior.h"
#include "TbbfCustomContext.h"

class TbbfMacroInstance;

class RespawnBehavior : public TbbfMacroBehavior {
private:
	void GoToMenu(TbbfMacroInstance* instance, TbbfCustomContext* context);
	void OpenMenu(TbbfMacroInstance* instance, TbbfCustomContext* context);
	void SpamSpawnIn(TbbfMacroInstance* instance, TbbfCustomContext* context);
protected:
	TickStatus TickTbbf(TbbfMacroInstance* instance, TbbfCustomContext* context, const FrameView& currentFrame) override;
public:
	RespawnBehavior() : TbbfMacroBehavior(16) {};
};