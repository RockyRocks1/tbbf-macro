#include <tbbf/behaviors/ToolBehavior.h>
#include <tbbf/TbbfMacroInstance.h>

TickStatus ToolBehavior::TickTbbf(TbbfMacroInstance* instance, TbbfContext* context, const FrameView& currentFrame) {
	TbbfContext::Decisions& decisions = context->decisions;
	m_debounceMs = 16;
	if (decisions.shouldEquipTool) {
		instance->SendKey(0x31);
		m_debounceMs = 300;
	}
	return TickStatus::Skipped;
}