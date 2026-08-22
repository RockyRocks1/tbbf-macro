#pragma once
#include <tbbf/behaviors/TbbfMacroBehavior.h>
#include <tbbf/context/TbbfContext.h>

class TbbfMacroInstance;

class GameMenuBehavior : public TbbfMacroBehavior {
private:
	void OpenMenu(TbbfMacroInstance* instance, TbbfContext* context);
protected:
	TickStatus TickTbbf(TbbfMacroInstance* instance, TbbfContext* context, const FrameView& currentFrame) override;
public:
	GameMenuBehavior() : TbbfMacroBehavior(16) {};
};