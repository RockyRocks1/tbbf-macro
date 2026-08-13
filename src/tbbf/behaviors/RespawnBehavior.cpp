#include <tbbf/behaviors/RespawnBehavior.h>
#include <tbbf/TbbfMacroInstance.h>

void RespawnBehavior::GoToMenu(TbbfMacroInstance* instance, TbbfContext* context) {
	POINT clickPosition = { context->uiLayout.inGameMenuAnchor.x - 100, context->uiLayout.inGameMenuAnchor.y + 100 };
	instance->ClickClient(clickPosition);
}
void RespawnBehavior::OpenMenu(TbbfMacroInstance* instance, TbbfContext* context) {
	POINT clickPosition = {context->uiLayout.inGameMenuAnchor.x - 10, context->uiLayout.inGameMenuAnchor.y + context->uiLayout.inGameMenuOpenOffset - 20};
	instance->ClickClient(clickPosition);
}
void RespawnBehavior::SpamSpawnIn(TbbfMacroInstance* instance, TbbfContext* context) {
	instance->SendKey(VK_RETURN);
	instance->ClickClient(context->spawnPosition, 16);
}
TickStatus RespawnBehavior::TickTbbf(TbbfMacroInstance* instance, TbbfContext* context, const FrameView& currentFrame) {
    if (context->decisions.currentTask == context->decisions.lastExecutedTask &&
        context->decisions.currentTask != TbbfMacroTask::Respawn_SpamSpawn)
        return TickStatus::Skipped;

    switch (context->decisions.currentTask) {
    case TbbfMacroTask::Respawn_GoToMenu:
        GoToMenu(instance, context);
        context->decisions.lastExecutedTask = TbbfMacroTask::Respawn_GoToMenu;
        m_debounceMs = 500;
        return TickStatus::Yield;
    case TbbfMacroTask::Respawn_SpamSpawn:
        SpamSpawnIn(instance, context);
        context->decisions.lastExecutedTask = TbbfMacroTask::Respawn_SpamSpawn;
        m_debounceMs = 16;
        return TickStatus::Yield;
    default:
	    return TickStatus::Skipped;
    }
}