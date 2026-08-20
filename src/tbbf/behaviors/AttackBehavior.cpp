#include <tbbf/behaviors/AttackBehavior.h>
#include <tbbf/TbbfMacroInstance.h>

TickStatus AttackBehavior::TickTbbf(TbbfMacroInstance* instance, TbbfContext* context, const FrameView& currentFrame) {
	if (context->decisions.shouldAttack)
		instance->ClickClient(POINT{ currentFrame.width / 2, currentFrame.height / 2 - 10 }, 10);

	return TickStatus::Skipped;
}