#pragma once
#include "TbbfMacroBehavior.h"
#include "TbbfCustomContext.h"

class TbbfMacroInstance;

class AttackBehavior : public TbbfMacroBehavior {
private:
	bool HasTool(const FrameView& currentFrame);
protected:
	TickStatus TickTbbf(TbbfMacroInstance* instance, TbbfCustomContext* context, const FrameView& currentFrame) override;
public:
	AttackBehavior() : TbbfMacroBehavior(16) {};
};
