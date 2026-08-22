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
	TbbfContext::Decisions& decisions = context->decisions;
	TbbfContext::BehaviorStatuses& statuses = context->statuses;

	if (!decisions.commandSelectTower) 
		return TickStatus::Skipped;

	switch (statuses.towerSelect) {
	case TowerSelectStatus::Idle:
		statuses.towerSelect = TowerSelectStatus::Selecting;
		return TickStatus::Redo;
	case TowerSelectStatus::Selecting:
		SelectTower(instance, context);
		statuses.towerSelect = TowerSelectStatus::Buying;
		m_debounceMs = 100;
		return TickStatus::Yield;
	case TowerSelectStatus::Buying:
		instance->SendKey(VK_SPACE);
		statuses.towerSelect = TowerSelectStatus::Equipping;
		m_debounceMs = 400;
		return TickStatus::Yield;
	case TowerSelectStatus::Equipping:
		instance->SendKey(VK_SPACE);
		statuses.towerSelect = TowerSelectStatus::Idle;
		context->lastProcessedWave = context->waveNumber;
		m_debounceMs = 400;
		return TickStatus::Yield;
	default:
		return TickStatus::Skipped;
	};
}