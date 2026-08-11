#include <tbbf/TowerSelectBehavior.h>
#include <tbbf/TbbfMacroInstance.h>


std::optional<int> TowerSelectBehavior::GetProperTowerIndex(TbbfCustomContext* context) {
	switch (context->waveNumber) {
	case 0:
		return context->towerListInfo.enforcerIndex;
	case 14:
		return context->towerListInfo.voidTraitorIndex;
	default:
		return std::nullopt;
	}
}

void TowerSelectBehavior::SelectTower(TbbfMacroInstance* instance, TbbfCustomContext* context) {
	std::optional<int> towerIndex = GetProperTowerIndex(context);
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

TickStatus TowerSelectBehavior::TickTbbf(TbbfMacroInstance* instance, TbbfCustomContext* context, const FrameView& currentFrame) {
	if (context->isBossPresent)
		return TickStatus::Skipped;
	if (context->towerListInfo.status != TowerListStatus::Done)
		return TickStatus::Skipped;

	if (context->playerStatus != PlayerStatus::Deployed && context->towerSelectStatus == TowerSelectStatus::Inactive)
		return TickStatus::Skipped;

	if (!IsWaveOfInterest(context->waveNumber))
		return TickStatus::Skipped;


	if (context->lastProcessedWave == context->waveNumber)
		return TickStatus::Skipped;

	switch (context->towerSelectStatus) {
	case TowerSelectStatus::Inactive:
		context->towerSelectStatus = TowerSelectStatus::Menuing;
		break;
	case TowerSelectStatus::Menuing:
		if (context->splashStatus != SplashTextStatus::Welcome)
			break;
		context->towerSelectStatus = TowerSelectStatus::Selecting;
		break;
	case TowerSelectStatus::Selecting:
		SelectTower(instance, context);
		context->towerSelectStatus = TowerSelectStatus::Buying;
		break;
	case TowerSelectStatus::Buying:
		instance->SendKey(VK_SPACE);
		context->towerSelectStatus = TowerSelectStatus::Equipping;
		break;
	case TowerSelectStatus::Equipping:
		instance->SendKey(VK_SPACE);
		context->towerSelectStatus = TowerSelectStatus::Inactive;
		context->lastProcessedWave = context->waveNumber;
		break;
	default:
		context->towerSelectStatus = TowerSelectStatus::Inactive;
		return TickStatus::Skipped;
	}
	return TickStatus::Yield;
}