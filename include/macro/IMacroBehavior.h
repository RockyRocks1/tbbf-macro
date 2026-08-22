#pragma once
#include <pixel/FrameView.h>
#include "IContext.h"

class IMacroInstance;

enum class TickStatus {
    Skipped,    // Skip this behavior;
    Yield,      // Skip all other behaviors after waiting;
    Redo,       // Do this exact behavior again immediately on the same frame;
    Terminated, // Terminate the macro instance immediately;
};

class IMacroBehavior {
public:
    virtual ~IMacroBehavior() = default;
    virtual bool CanTick(uint64_t currentTimestampMs) const = 0;
    virtual void UpdateLastTickTime(uint64_t tickTime) = 0;
    virtual TickStatus Tick(IMacroInstance* instance, IContext* context, const FrameView& frame) = 0;
};