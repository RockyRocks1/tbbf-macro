#pragma once
#include <tbbf/behaviors/TbbfMacroBehavior.h>
#include <tbbf/context/TbbfContext.h>

class TbbfMacroInstance;

class GameLoadedBehavior : public TbbfMacroBehavior {
protected:
	TickStatus TickTbbf(TbbfMacroInstance* instance, TbbfContext* context, const FrameView& currentFrame) override;
public:
	GameLoadedBehavior() : TbbfMacroBehavior(16) {};
};