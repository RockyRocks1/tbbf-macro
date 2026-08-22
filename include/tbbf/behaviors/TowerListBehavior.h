#pragma once
#include <tbbf/behaviors/TbbfMacroBehavior.h>
#include <tbbf/context/TbbfContext.h>
#include <tbbf/parsers/TowerListReader.h>

class TbbfMacroInstance;

class TowerListBehavior : public TbbfMacroBehavior {
private:
	void CalibrateStart(TbbfMacroInstance* instance);
	void ProcessTowerListReading(TbbfMacroInstance* instance, TbbfContext* context, const FrameView& currentFrame);
protected:
	TickStatus TickTbbf(TbbfMacroInstance* instance, TbbfContext* context, const FrameView& currentFrame) override;
public:
	TowerListBehavior() : TbbfMacroBehavior(16) {};
};
