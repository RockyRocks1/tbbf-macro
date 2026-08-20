#include <tbbf/behaviors/GameMenuBehavior.h>
#include <tbbf/TbbfMacroInstance.h>
void GameMenuBehavior::OpenMenu(TbbfMacroInstance* instance, TbbfContext* context) {
    POINT clickPosition = { context->uiLayout.inGameMenuAnchor.x - 10, context->uiLayout.inGameMenuAnchor.y + context->uiLayout.inGameMenuOpenOffset - 20 };
    instance->ClickClient(clickPosition);
}
TickStatus GameMenuBehavior::TickTbbf(TbbfMacroInstance* instance, TbbfContext* context, const FrameView& currentFrame) {
	m_debounceMs = 16;
	if (context->decisions.shouldOpenMenu) {
		OpenMenu(instance, context);
		m_debounceMs = 1000;
	}
    return TickStatus::Skipped;
}