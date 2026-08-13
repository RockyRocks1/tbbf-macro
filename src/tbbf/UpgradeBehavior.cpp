#include <tbbf/UpgradeBehavior.h>
#include <tbbf/TbbfMacroInstance.h>

TickStatus UpgradeBehavior::TickTbbf(TbbfMacroInstance* instance, TbbfCustomContext* context, const FrameView& currentFrame) {
	m_debounceMs = 16;
	if (context->playerStatus != PlayerStatus::Menu)
		return TickStatus::Skipped;
	if (context->towerSelectStatus != TowerSelectStatus::Inactive)
		return TickStatus::Skipped;
	

	instance->SendKey(VK_SPACE);
	m_debounceMs = 3000;
	return TickStatus::Skipped;
}