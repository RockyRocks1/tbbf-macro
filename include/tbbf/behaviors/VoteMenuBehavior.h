#pragma once
#include <tbbf/behaviors/TbbfMacroBehavior.h>
#include <tbbf/context/TbbfContext.h>

class TbbfMacroInstance;

class VoteMenuBehavior : public TbbfMacroBehavior {
private:
	void VoteBlockyCastle(TbbfMacroInstance* instance);
	void VoteGamemode(TbbfMacroInstance* instance, TargetGamemode gamemode);
protected:
	TickStatus TickTbbf(TbbfMacroInstance* instance, TbbfContext* context, const FrameView& currentFrame) override;
public:
	VoteMenuBehavior() : TbbfMacroBehavior(16) {};
};