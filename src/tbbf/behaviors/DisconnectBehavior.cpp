#include <tbbf/behaviors/ShutdownBehavior.h>
#include <tbbf/TbbfMacroInstance.h>

TickStatus ShutdownBehavior::TickTbbf(TbbfMacroInstance* instance, TbbfContext* context, const FrameView& currentFrame) {
    if (context->decisions.currentTask == TbbfMacroTask::Shutdown)
        return TickStatus::Terminated;

    return TickStatus::Skipped;
}