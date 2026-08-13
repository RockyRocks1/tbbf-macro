#include <tbbf/behaviors/GameLoadedBehavior.h>
#include <tbbf/TbbfMacroInstance.h>

TickStatus GameLoadedBehavior::TickTbbf(TbbfMacroInstance* instance, TbbfContext* context, const FrameView& currentFrame) {
    if (context->decisions.currentTask == TbbfMacroTask::Game_WaitForLoad)
        return TickStatus::Yield;

    return TickStatus::Skipped;
}