#include <tbbf/behaviors/RespawnBehavior.h>
#include <tbbf/TbbfMacroInstance.h>

void RespawnBehavior::GoToMenu(TbbfMacroInstance* instance, TbbfContext* context) {
    int i = 1;
    switch (i) {
    case 0: {
        POINT clickPosition = { context->uiLayout.inGameMenuAnchor.x - 100, context->uiLayout.inGameMenuAnchor.y + 100 };
        instance->ClickClient(clickPosition, 50);
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

TickStatus RespawnBehavior::TickTbbf(TbbfMacroInstance* instance, TbbfContext* context, const FrameView& currentFrame) {
    m_debounceMs = 16;
    if (context->decisions.commandGoToMenu) {
        GoToMenu(instance, context);
        m_debounceMs = 5000;
        return TickStatus::Skipped;
    }
   

    return TickStatus::Skipped;
}