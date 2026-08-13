#pragma once
#include "TbbfMacroBehavior.h"
#include "TbbfCustomContext.h"

class TbbfMacroInstance;

class UpgradeBehavior : public TbbfMacroBehavior {
protected:
	TickStatus TickTbbf(TbbfMacroInstance* instance, TbbfCustomContext* context, const FrameView& currentFrame) override;
public:
	UpgradeBehavior() : TbbfMacroBehavior(16) {};
};