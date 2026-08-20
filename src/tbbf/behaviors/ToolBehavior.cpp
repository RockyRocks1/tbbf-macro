#include <tbbf/behaviors/ToolBehavior.h>
#include <tbbf/TbbfMacroInstance.h>

TickStatus ToolBehavior::TickTbbf(TbbfMacroInstance* instance, TbbfContext* context, const FrameView& currentFrame) {
	m_debounceMs = 16;
	if (context->decisions.shouldEquipTool) {
		instance->SendKey(0x31);
		m_debounceMs = 300;
	}
	return TickStatus::Skipped;
}