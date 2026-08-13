#include <tbbf/behaviors/UpgradeBehavior.h>
#include <tbbf/TbbfMacroInstance.h>

TickStatus UpgradeBehavior::TickTbbf(TbbfMacroInstance* instance, TbbfContext* context, const FrameView& currentFrame) {
	m_debounceMs = 16;
	if (!context->decisions.shouldUpgrade)
		return TickStatus::Skipped;
	instance->SendKey(VK_SPACE);
	m_debounceMs = 3000;
	return TickStatus::Skipped;
}