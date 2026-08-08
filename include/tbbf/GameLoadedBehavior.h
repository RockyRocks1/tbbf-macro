#pragma once
#include "TbbfMacroBehavior.h"
#include "TbbfCustomContext.h"
#include <pixel/UDim2.h>

class TbbfMacroInstance;

class GameLoadedBehavior : public TbbfMacroBehavior {
protected:
	TickStatus TickTbbf(TbbfMacroInstance* instance, TbbfCustomContext* context, const FrameView& currentFrame) override;
public:
	GameLoadedBehavior() : TbbfMacroBehavior(16) {};
};