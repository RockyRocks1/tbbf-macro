#include <tbbf/RespawnBehavior.h>
#include <tbbf/TbbfMacroInstance.h>

void RespawnBehavior::GoToMenu(TbbfMacroInstance* instance, TbbfCustomContext* context) {
	POINT clickPosition = { context->uiLayout.inGameMenuAnchor.x - 100, context->uiLayout.inGameMenuAnchor.y + 100 };
	instance->ClickClient(clickPosition);
}
void RespawnBehavior::OpenMenu(TbbfMacroInstance* instance, TbbfCustomContext* context) {
	POINT clickPosition = {context->uiLayout.inGameMenuAnchor.x - 10, context->uiLayout.inGameMenuAnchor.y + context->uiLayout.inGameMenuOpenOffset - 20};
	
	instance->ClickClient(clickPosition);
}
void RespawnBehavior::SpamSpawnIn(TbbfMacroInstance* instance, TbbfCustomContext* context) {
	instance->SendKey(VK_RETURN);
	instance->SendKey(VK_SPACE);
	instance->ClickClient(context->spawnPosition, 16);
}
TickStatus RespawnBehavior::TickTbbf(TbbfMacroInstance* instance, TbbfCustomContext* context, const FrameView& currentFrame) {
	m_debounceMs = 16;
	if (context->playerStatus == PlayerStatus::Loading || context->towerListInfo.status != TowerListStatus::Done)
		return TickStatus::Skipped;
	
	if (context->playerStatus == PlayerStatus::Deployed) {
		OpenMenu(instance, context);
		if (!context->inGameMenuOpen) {
			m_debounceMs = 1000;
			context->inGameMenuOpen = true;
			return TickStatus::Yield;
		}
	}
	if (context->towerSelectStatus == TowerSelectStatus::Inactive &&
		(context->playerStatus == PlayerStatus::Respawning || context->playerStatus == PlayerStatus::Menu)) {
		SpamSpawnIn(instance, context);
		return TickStatus::Yield;
	}

	if (context->playerStatus != PlayerStatus::Respawning &&
		(context->towerSelectStatus == TowerSelectStatus::Menuing || context->playerStatus == PlayerStatus::Dead)) {
		context->playerStatus = PlayerStatus::Respawning;
		GoToMenu(instance, context);
		return TickStatus::Yield;
	}
	

	return TickStatus::Skipped;
}