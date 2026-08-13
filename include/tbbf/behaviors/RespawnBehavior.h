#pragma once
#include <tbbf/behaviors/TbbfMacroBehavior.h>
#include <tbbf/context/TbbfContext.h>

class TbbfMacroInstance;

class RespawnBehavior : public TbbfMacroBehavior {
private:
	void GoToMenu(TbbfMacroInstance* instance, TbbfContext* context);
	void OpenMenu(TbbfMacroInstance* instance, TbbfContext* context);
	void SpamSpawnIn(TbbfMacroInstance* instance, TbbfContext* context);
protected:
	TickStatus TickTbbf(TbbfMacroInstance* instance, TbbfContext* context, const FrameView& currentFrame) override;
public:
	RespawnBehavior() : TbbfMacroBehavior(16) {};
};