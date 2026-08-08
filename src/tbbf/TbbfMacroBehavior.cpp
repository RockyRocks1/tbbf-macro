#include <tbbf/TbbfMacroBehavior.h>
#include <tbbf/TbbfMacroInstance.h>
#include <tbbf/TbbfCustomContext.h>

TickStatus TbbfMacroBehavior::Tick(IMacroInstance* instance, IContext* context, const FrameView& currentFrame) {
    TbbfMacroInstance* tbbfInstance = static_cast<TbbfMacroInstance*>(instance);
    TbbfCustomContext* tbbfContext = static_cast<TbbfCustomContext*>(context);

    return TickTbbf(tbbfInstance, tbbfContext, currentFrame);
}