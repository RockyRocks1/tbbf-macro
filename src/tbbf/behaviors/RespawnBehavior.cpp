#include <tbbf/behaviors/RespawnBehavior.h>
#include <tbbf/TbbfMacroInstance.h>

void RespawnBehavior::GoToMenu(TbbfMacroInstance* instance, TbbfContext* context) {
    int i = 1;
    switch (i) {
    case 0: {
        POINT clickPosition = { context->uiLayout.inGameMenuAnchor.x - 100, context->uiLayout.inGameMenuAnchor.y + 100 };
        instance->ClickClient(clickPosition);
        break;
    }
    case 1: {
        instance->ToggleUiFocus();
        instance->SendKey(VK_DOWN);
        instance->ToggleUiFocus();
        instance->ToggleUiFocus();
        instance->SendKey(VK_DOWN);
        instance->SendKey(VK_RETURN);
        instance->ToggleUiFocus();
        break;
    }
    default:
        break;
    }
}
void RespawnBehavior::SpawnIn(TbbfMacroInstance* instance, TbbfContext* context) {
	instance->SendKey(VK_RETURN);
	instance->ClickClient(context->spawnPosition, 16);
}
TickStatus RespawnBehavior::TickTbbf(TbbfMacroInstance* instance, TbbfContext* context, const FrameView& currentFrame) {
    if (context->decisions.currentTask == context->decisions.lastExecutedTask &&
        context->decisions.currentTask != TbbfMacroTask::Respawn_Spawning)
        return TickStatus::Skipped;

    switch (context->decisions.currentTask) {
    case TbbfMacroTask::Respawn_GoToMenu:
        GoToMenu(instance, context);
        context->decisions.lastExecutedTask = TbbfMacroTask::Respawn_GoToMenu;
        m_debounceMs = 500;
        return TickStatus::Yield;
    case TbbfMacroTask::Respawn_Spawning:
        SpawnIn(instance, context);
        context->decisions.lastExecutedTask = TbbfMacroTask::Respawn_Spawning;
        m_debounceMs = 16;
        return TickStatus::Skipped;
    default:
	    return TickStatus::Skipped;
    }
}