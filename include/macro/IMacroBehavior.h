#pragma once
#include <pixel/FrameView.h>

class IMacroInstance;

enum class TickStatus {
    Handled,    // Action taken, skip all other behaviors after waiting;
    Skipped,    // No action taken, skip this behavior;
    Yield,      // No action taken, skip all other behaviors after waiting;
    Terminated, // Action taken, terminate the macro instance;
};

struct TickResult {
    TickStatus status = TickStatus::Skipped;
    int waitTimeMs = 16;
};

class IMacroBehavior {
public:
	virtual ~IMacroBehavior() = default;

	virtual TickResult OnTick(IMacroInstance* instance, const FrameView& currentFrame) = 0;
};