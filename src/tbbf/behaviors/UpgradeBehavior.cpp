#include <tbbf/behaviors/UpgradeBehavior.h>
#include <tbbf/TbbfMacroInstance.h>

TickStatus UpgradeBehavior::TickTbbf(TbbfMacroInstance* instance, TbbfContext* context, const FrameView& currentFrame) {
	TbbfContext::Decisions& decisions = context->decisions;
	m_debounceMs = 16;
	if (decisions.shouldUpgrade) {
		instance->SendKey(VK_SPACE);
		m_debounceMs = 3000;
	}
	return TickStatus::Skipped;
}