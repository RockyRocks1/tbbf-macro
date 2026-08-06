#pragma once
#include <macro/IMacroBehavior.h>

class TbbfMacroInstance;

class DisconnectBehavior : public IMacroBehavior {
public:
	DisconnectBehavior() = default;
	~DisconnectBehavior() override = default;
	
	TickResult OnTick(IMacroInstance* instance, const FrameView& currentFrame) override;
};