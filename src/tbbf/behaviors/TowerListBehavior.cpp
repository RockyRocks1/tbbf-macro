#include <tbbf/behaviors/TowerListBehavior.h>
#include <tbbf/TbbfMacroInstance.h>

void TowerListBehavior::CalibrateStart(TbbfMacroInstance* instance) {
    instance->ToggleUiFocus();
    instance->SendKey(VK_DOWN);
    instance->ToggleUiFocus();
    instance->ToggleUiFocus();
    instance->SendKey(VK_UP);
}

void TowerListBehavior::ProcessTowerListReading(TbbfMacroInstance* instance, TbbfContext* context, const FrameView& currentFrame) {
    TbbfContext::TowerListInfo& info = context->towerList;
    static const std::vector<std::string> towerList{
        "Scout", "Sniper", "Fragger", "Shotgunner", "Cryo-Gunner", "Enforcer", "Patrioteer",
        "Snowballer", "Tweeter", "Soldier", "Patrol", "Aviator", "Knifer",
        "Doctor", "Tuber", "Elf", "Mercenary", "Golden Scout", "Barracks",
        "Marksman", "Archer", "Engineer", "Flamethrower", "Commander",
        "Plasma Trooper", "Mortar", "Commando", "Spiritual Advocate",
        "Hallowboomer",
        "Railgunner", "Void Traitor", "Phaser", "Golden Commando", "Zed", "Golden Zed"
    }; // THIS IS TEMPORARY
    std::optional<std::string> towerName = TowerListReader::IdentifyTower(currentFrame, towerList);
    
    if (!towerName) {
        info.status = TowerListStatus::Ready;
        instance->ToggleUiFocus();
        return;
    }

    instance->SendKey(VK_DOWN);

    if (!towerName->empty())
        info.registry[*towerName] = info.currentReadIndex;

    info.currentReadIndex++;
}

TickStatus TowerListBehavior::TickTbbf(TbbfMacroInstance* instance, TbbfContext* context, const FrameView& currentFrame) {
    if (context->decisions.currentTask == context->decisions.lastExecutedTask &&
        context->decisions.currentTask != TbbfMacroTask::TowerList_ReadingItem)
        return TickStatus::Skipped;

    switch (context->decisions.currentTask) {
    case TbbfMacroTask::TowerList_Initializing:
        CalibrateStart(instance);

        context->decisions.lastExecutedTask = TbbfMacroTask::TowerList_Initializing;

        m_debounceMs = 500;
        return TickStatus::Yield;

    case TbbfMacroTask::TowerList_ReadingItem:
        m_debounceMs = 500;
        ProcessTowerListReading(instance, context, currentFrame);
        context->decisions.lastExecutedTask = TbbfMacroTask::TowerList_ReadingItem;

        return TickStatus::Yield;

    default:
        return TickStatus::Skipped;
    }
}
