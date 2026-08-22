#include <tbbf/behaviors/GameMenuBehavior.h>
#include <tbbf/TbbfMacroInstance.h>
void GameMenuBehavior::OpenMenu(TbbfMacroInstance* instance, TbbfContext* context) {
    POINT clickPosition = { context->uiLayout.inGameMenuAnchor.x - 10, context->uiLayout.inGameMenuAnchor.y + context->uiLayout.inGameMenuOpenOffset - 20 };
    instance->ClickClient(clickPosition, 10);
}
TickStatus GameMenuBehavior::TickTbbf(TbbfMacroInstance* instance, TbbfContext* context, const FrameView& currentFrame) {
	TbbfContext::Decisions& decisions = context->decisions;
	m_debounceMs = 16;
	if (decisions.shouldOpenMenu) {
		OpenMenu(instance, context);
		m_debounceMs = 500;
	}
    return TickStatus::Skipped;
}