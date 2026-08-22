#pragma once
#include <tbbf/behaviors/TbbfMacroBehavior.h>
#include <tbbf/context/TbbfContext.h>

class TbbfMacroInstance;

class DeployBehavior : public TbbfMacroBehavior {
private:
	void Deploy(TbbfMacroInstance* instance, TbbfContext* context);
protected:
	TickStatus TickTbbf(TbbfMacroInstance* instance, TbbfContext* context, const FrameView& currentFrame) override;
public:
	DeployBehavior() : TbbfMacroBehavior(16) {};
};