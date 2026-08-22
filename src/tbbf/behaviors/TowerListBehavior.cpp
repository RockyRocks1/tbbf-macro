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
    TbbfContext::BehaviorStatuses& statuses = context->statuses;
    static const std::vector<std::string> towerList{
        "Scout", "Sniper", "Fragger", "Shotgunner", "Cryo-Gunner", "Enforcer",
        "Patrioteer", "Snowballer", "Tweeter", "Soldier", "Patrol", "Aviator", 
        "Knifer", "Doctor", "Tuber", "Elf", "Mercenary", "Golden Scout", 
        "Barracks", "Marksman", "Archer", "Engineer", "Flamethrower", "Commander",
        "Plasma Trooper", "Mortar", "Commando", "Spiritual Advocate", "Hallowboomer",
        "Exterminator", "Ice Apprentice", "Huntsman", "Railgunner", "Void Traitor", "Phaser",
        "Golden Commando", "Zed", "Golden Zed"
    }; // THIS IS TEMPORARY, MOVE TO JSON LATER
    std::optional<std::string> towerName = TowerListReader::IdentifyTower(currentFrame, towerList);

    if (!towerName) {
        statuses.towerList = TowerListStatus::Ready;
        instance->ToggleUiFocus();
        return;
    }

    instance->SendKey(VK_DOWN);

    if (!towerName->empty())
        info.registry[*towerName] = info.currentReadIndex;

    info.currentReadIndex++;
}

TickStatus TowerListBehavior::TickTbbf(TbbfMacroInstance* instance, TbbfContext* context, const FrameView& currentFrame) {
    TbbfContext::Decisions& decisions = context->decisions;
    TbbfContext::BehaviorStatuses& statuses = context->statuses;
    TbbfContext::TowerListInfo& info = context->towerList;

    if (!decisions.commandReadTowerList)
        return TickStatus::Skipped;

    switch (statuses.towerList) {
    case TowerListStatus::Idle:
        CalibrateStart(instance);
        statuses.towerList = TowerListStatus::Reading;
        m_debounceMs = 500;
        return TickStatus::Yield;
    case TowerListStatus::Reading:
        ProcessTowerListReading(instance, context, currentFrame);
        m_debounceMs = 500; 
        return TickStatus::Yield;
    case TowerListStatus::Ready:
    default:
        return TickStatus::Skipped;
    }
}
