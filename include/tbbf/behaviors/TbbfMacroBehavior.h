#pragma once
#include <macro/BaseMacroBehavior.h>


class TbbfMacroInstance;
struct TbbfContext;

class TbbfMacroBehavior : public BaseMacroBehavior {
public:
    using BaseMacroBehavior::BaseMacroBehavior;

    TickStatus Tick(IMacroInstance* instance, IContext* context, const FrameView& currentFrame) final;

protected:
    virtual TickStatus TickTbbf(TbbfMacroInstance* instance, TbbfContext* context, const FrameView& currentFrame) = 0;
};