#include <tbbf/behaviors/TowerListBehavior.h>
#include <tbbf/parsers/TowerListReader.h>
#include <tbbf/TbbfMacroInstance.h>

void TowerListBehavior::CalibrateStart(TbbfMacroInstance* instance) {
    instance->ToggleUiFocus();
    instance->SendKey(VK_DOWN);
    instance->ToggleUiFocus();
    instance->ToggleUiFocus();
    instance->SendKey(VK_UP);
}

void TowerListBehavior::ProcessTowerListReading(TbbfMacroInstance* instance, TbbfContext* context, const FrameView& currentFrame) {
    std::optional<const Rect> navBox = TowerListReader::FindNavBoxWithinTowerList(currentFrame);
    TbbfContext::TowerListInfo& info = context->towerList;

    if (!navBox) {
        info.status = TowerListStatus::Ready;
        instance->ToggleUiFocus();
        return;
    }

    instance->SendKey(VK_DOWN);

    std::string towerName = TowerListReader::ReadTextWithinNavBox(currentFrame, *navBox);

    if (!towerName.empty())
        info.registry[towerName] = info.currentReadIndex;

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
