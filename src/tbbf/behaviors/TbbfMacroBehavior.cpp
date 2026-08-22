#include <tbbf/behaviors/TbbfMacroBehavior.h>
#include <tbbf/TbbfMacroInstance.h>
#include <tbbf/context/TbbfContext.h>

TickStatus TbbfMacroBehavior::Tick(IMacroInstance* instance, IContext* context, const FrameView& currentFrame) {
    TbbfMacroInstance* tbbfInstance = static_cast<TbbfMacroInstance*>(instance);
    TbbfContext* tbbfContext = static_cast<TbbfContext*>(context);

    return TickTbbf(tbbfInstance, tbbfContext, currentFrame);
}