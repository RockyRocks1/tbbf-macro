#pragma once
#include "TbbfMacroBehavior.h"
#include "TbbfCustomContext.h"


enum class Gamemode {
	Classic = 0,
	Hardmode = 1
};
class TbbfMacroInstance;

class VoteMenuBehavior : public TbbfMacroBehavior {
private:
	std::optional<POINT> FindSkipVote(const FrameView& currentFrame);
	void VoteBlockyCastle(TbbfMacroInstance* instance, const FrameView& currentFrame);
	void VoteGamemode(TbbfMacroInstance* instance, const FrameView& currentFrame, Gamemode gamemode = Gamemode::Classic);
protected:
	TickStatus TickTbbf(TbbfMacroInstance* instance, TbbfCustomContext* context, const FrameView& currentFrame) override;
public:
	VoteMenuBehavior() : TbbfMacroBehavior(16) {};
};