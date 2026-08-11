#pragma once
#include "TbbfMacroBehavior.h"
#include "TbbfCustomContext.h"

class TbbfMacroInstance;

class TowerSelectBehavior : public TbbfMacroBehavior {
private:
	std::optional<int> GetProperTowerIndex(TbbfCustomContext* context);
	void SelectTower(TbbfMacroInstance* instance, TbbfCustomContext* context);
	inline bool IsWaveOfInterest(int waveNumber) {
		return waveNumber == 0 || waveNumber == 14;
	}
protected:
	TickStatus TickTbbf(TbbfMacroInstance* instance, TbbfCustomContext* context, const FrameView& currentFrame) override;
public:
	TowerSelectBehavior() : TbbfMacroBehavior(250) {};
};