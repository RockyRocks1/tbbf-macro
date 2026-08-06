#pragma once
#include <macro/IMacroBehavior.h>
#include <pixel/UDim2.h>
#include <pixel/PixelAnalyzer.h>

class TbbfMacroInstance;

class GameLoadedBehavior : public IMacroBehavior {
public:
	GameLoadedBehavior() = default;
	~GameLoadedBehavior() override = default;

	TickResult OnTick(IMacroInstance* instance, const FrameView& currentFrame) override;
};