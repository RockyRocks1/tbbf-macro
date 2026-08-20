#include <tbbf/behaviors/VoteMenuBehavior.h>
#include <tbbf/TbbfMacroInstance.h>

void VoteMenuBehavior::VoteBlockyCastle(TbbfMacroInstance* instance) {
	static constexpr int blockyCastleIndex = 2;
	instance->ToggleUiFocus();
	instance->SendKey(VK_DOWN);
	instance->ToggleUiFocus();

	instance->ToggleUiFocus();
	instance->SendKey(VK_DOWN);

	instance->RepeatKey(VK_RIGHT, blockyCastleIndex);
	instance->SendKey(VK_RETURN);
	instance->ToggleUiFocus();
}
void VoteMenuBehavior::VoteGamemode(TbbfMacroInstance* instance, TargetGamemode gamemode) {
	int gamemodeIndex;
	switch (gamemode) {
		case TargetGamemode::Hardmode:
			gamemodeIndex = 1;
			break;
		case TargetGamemode::Classic:
		default:
			gamemodeIndex = 0;
			break;
	};
	
	instance->ToggleUiFocus();
	instance->SendKey(VK_DOWN);
	instance->ToggleUiFocus();

	instance->ToggleUiFocus();
	instance->SendKey(VK_DOWN);

	instance->RepeatKey(VK_RIGHT, gamemodeIndex);
	instance->SendKey(VK_RETURN);
	instance->ToggleUiFocus();
}
TickStatus VoteMenuBehavior::TickTbbf(TbbfMacroInstance* instance, TbbfContext* context, const FrameView& currentFrame) {
	if (context->decisions.currentTask == context->decisions.lastExecutedTask)
		return TickStatus::Skipped;
	switch (context->decisions.currentTask) {
	case TbbfMacroTask::Vote_SubmittingMap:
		VoteBlockyCastle(instance);
		if (context->uiLayout.skipVoteButtonPos)
			instance->ClickClient(*context->uiLayout.skipVoteButtonPos);
		context->decisions.lastExecutedTask = TbbfMacroTask::Vote_SubmittingMap;
		m_debounceMs = 500;
		return TickStatus::Yield;
	case TbbfMacroTask::Vote_SubmittingGamemode:
		VoteGamemode(instance, TargetGamemode::Classic);
		if (context->uiLayout.skipVoteButtonPos)
			instance->ClickClient(*context->uiLayout.skipVoteButtonPos);
		context->decisions.lastExecutedTask = TbbfMacroTask::Vote_SubmittingGamemode;
		m_debounceMs = 500;
		return TickStatus::Yield;
	case TbbfMacroTask::Vote_WaitingForMatch:
	case TbbfMacroTask::Vote_WaitForLoad:
		return TickStatus::Yield;
	default:
		return TickStatus::Skipped;
	}
}