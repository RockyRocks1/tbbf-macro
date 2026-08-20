#include <tbbf/behaviors/TowerSelectBehavior.h>
#include <tbbf/TbbfMacroInstance.h>

void TowerSelectBehavior::SelectTower(TbbfMacroInstance* instance, TbbfContext* context) {
	std::optional<int> towerIndex = std::nullopt;
	if (context->waveNumber == 0)
		towerIndex = context->towerList.GetTowerIndex("Enforcer");
	else if (context->waveNumber == 14)
		towerIndex = context->towerList.GetTowerIndex("Void Traitor");

	if (!towerIndex)
		return;

	instance->ToggleUiFocus();
	instance->SendKey(VK_DOWN);
	instance->ToggleUiFocus();
	instance->ToggleUiFocus();
	instance->SendKey(VK_UP);
	instance->RepeatKey(VK_DOWN, *towerIndex);
	instance->SendKey(VK_RETURN);
	instance->ToggleUiFocus();
}

TickStatus TowerSelectBehavior::TickTbbf(TbbfMacroInstance* instance, TbbfContext* context, const FrameView& currentFrame) {
	if (context->decisions.currentTask == context->decisions.lastExecutedTask)
		return TickStatus::Skipped;

	switch (context->decisions.currentTask) {
	case TbbfMacroTask::TowerSelect_Selecting:
		m_debounceMs = 100;
		SelectTower(instance, context);
		context->decisions.lastExecutedTask = TbbfMacroTask::TowerSelect_Selecting;
		return TickStatus::Yield;
	case TbbfMacroTask::TowerSelect_Buying:
		m_debounceMs = 400;
		instance->SendKey(VK_SPACE);
		context->decisions.lastExecutedTask = TbbfMacroTask::TowerSelect_Buying;
		return TickStatus::Yield;
	case TbbfMacroTask::TowerSelect_Equipping:
		m_debounceMs = 400;
		instance->SendKey(VK_SPACE);
		context->decisions.lastExecutedTask = TbbfMacroTask::TowerSelect_Equipping;
		context->lastProcessedWave = context->waveNumber;
		return TickStatus::Yield;
	default:
		return TickStatus::Skipped;
	}
}