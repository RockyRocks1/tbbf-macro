#pragma once
#include <tbbf/behaviors/TbbfMacroBehavior.h>
#include <tbbf/context/TbbfContext.h>

class TbbfMacroInstance;

class TowerSelectBehavior : public TbbfMacroBehavior {
private:
	void SelectTower(TbbfMacroInstance* instance, TbbfContext* context);
protected:
	TickStatus TickTbbf(TbbfMacroInstance* instance, TbbfContext* context, const FrameView& currentFrame) override;
public:
	TowerSelectBehavior() : TbbfMacroBehavior(250) {};
};