#pragma once
#include <tbbf/behaviors/TbbfMacroBehavior.h>
#include <tbbf/context/TbbfContext.h>

class TbbfMacroInstance;

class RespawnBehavior : public TbbfMacroBehavior {
private:
	void GoToMenu(TbbfMacroInstance* instance, TbbfContext* context);
	void SpawnIn(TbbfMacroInstance* instance, TbbfContext* context);
protected:
	TickStatus TickTbbf(TbbfMacroInstance* instance, TbbfContext* context, const FrameView& currentFrame) override;
public:
	RespawnBehavior() : TbbfMacroBehavior(16) {};
};