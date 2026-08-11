#pragma once
#include "TbbfMacroBehavior.h"
#include "TbbfCustomContext.h"
#include <pixel/UDim2.h>

class TbbfMacroInstance;

class WaveChangedBehavior : public TbbfMacroBehavior {
private:
	bool WaveTextExists(const FrameView& waveTextFrame);
	std::string ParseWaveText(const FrameView& waveTextFrame);
	std::optional<int> SanitizeWaveText(std::string rawWaveText);
protected:
	TickStatus TickTbbf(TbbfMacroInstance* instance, TbbfCustomContext* context, const FrameView& currentFrame) override;
public:
	WaveChangedBehavior() : TbbfMacroBehavior(16) {};
};