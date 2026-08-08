#pragma once
#include "TbbfMacroBehavior.h"
#include "TbbfCustomContext.h"
#include <pixel/UDim2.h>

class TbbfMacroInstance;

class CentralContextBehavior : public TbbfMacroBehavior {
private:
	SplashTextStatus GetSplashTextStatus(TbbfMacroInstance* instance, const FrameView& currentFrame);
protected:
	TickStatus TickTbbf(TbbfMacroInstance* instance, TbbfCustomContext* context, const FrameView& currentFrame) override;
public:
	CentralContextBehavior() : TbbfMacroBehavior(16) {};
};
