#include <tbbf/behaviors/ShutdownBehavior.h>
#include <tbbf/TbbfMacroInstance.h>

TickStatus ShutdownBehavior::TickTbbf(TbbfMacroInstance* instance, TbbfContext* context, const FrameView& currentFrame) {
    TbbfContext::Decisions& decisions = context->decisions;
    if (decisions.commandShutdown)
        return TickStatus::Terminated;

    return TickStatus::Skipped;
}