#pragma once
#include <tbbf/behaviors/TbbfMacroBehavior.h>
#include <tbbf/context/TbbfContext.h>

class TbbfMacroInstance;

class CentralContextBehavior : public TbbfMacroBehavior {
protected:
	TickStatus TickTbbf(TbbfMacroInstance* instance, TbbfContext* context, const FrameView& currentFrame) override;
public:
	CentralContextBehavior() : TbbfMacroBehavior(16) {};
};
