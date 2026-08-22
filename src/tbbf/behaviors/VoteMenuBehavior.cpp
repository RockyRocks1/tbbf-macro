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
	TbbfContext::Decisions& decisions = context->decisions;
	TbbfContext::BehaviorStatuses& statuses = context->statuses;

	if (!decisions.commandVoteMenu)
		return TickStatus::Skipped;

	switch (statuses.voteMenu) {
	case VoteMenuStatus::Idle:
		statuses.voteMenu = VoteMenuStatus::VotingMap;
		m_debounceMs = 4000;
		return TickStatus::Yield;
	case VoteMenuStatus::VotingMap:
		VoteBlockyCastle(instance);
		statuses.voteMenu = VoteMenuStatus::VotingGamemode;
		m_debounceMs = 25000;
		return TickStatus::Yield;
	case VoteMenuStatus::VotingGamemode:
		VoteGamemode(instance, TargetGamemode::Hardmode);
		decisions.isWaitingForMenu = true;
		statuses.voteMenu = VoteMenuStatus::Idle;
		m_debounceMs = 16;
		return TickStatus::Yield;
	default:
		return TickStatus::Skipped;
	}
}