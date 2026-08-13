#include <tbbf/context/TbbfContext.h>
#include <tbbf/behaviors/TbbfMacroBehavior.h>


class TbbfMacroInstance;

class MainDecisionBehavior : public TbbfMacroBehavior {
protected:
	TickStatus TickTbbf(TbbfMacroInstance* instance, TbbfContext* context, const FrameView& currentFrame) override;
public:
	MainDecisionBehavior() : MainDecisionBehavior(16) {};
};

