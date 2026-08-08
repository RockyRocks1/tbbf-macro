#include <tbbf/VoteMenuBehavior.h>
#include <tbbf/TbbfMacroInstance.h>

std::optional<POINT> VoteMenuBehavior::FindSkipVote(const FrameView& frame) {
	static constexpr UDim2 skipVoteUDimPosition = UDim2(1.0f, -50, 1.0f, -75);
	static constexpr UDim2 skipVoteUDimSize = UDim2(0.f, 50, 0.f, 25);
	static constexpr ColorRgba targetWhiteColor = { 255, 255, 255 };

	const Size2D frameSize{ frame.width, frame.height };

	const POINT buttonPosition = skipVoteUDimPosition.Resolve(frameSize);
	const POINT buttonSize = skipVoteUDimSize.Resolve(frameSize);
	const POINT leftTopPosition{ buttonPosition.x - buttonSize.x / 2, buttonPosition.y - buttonSize.y / 2 };
	
	const FrameView& skipVoteCropped = PixelModifier::Crop(frame, leftTopPosition.x, leftTopPosition.y, buttonSize.x, buttonSize.y);
	std::optional<int> whitePixels = PixelAnalyzer::FindPixelOccurrences(skipVoteCropped, targetWhiteColor);

	if (!whitePixels || *whitePixels < 100)
		return std::nullopt;

	return buttonPosition;
}

void VoteMenuBehavior::VoteBlockyCastle(TbbfMacroInstance* instance, const FrameView& currentFrame) {
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
void VoteMenuBehavior::VoteGamemode(TbbfMacroInstance* instance, const FrameView& currentFrame, Gamemode gamemode) {
	int gamemodeIndex;
	switch (gamemode) {
		case Gamemode::Hardmode:
			gamemodeIndex = 1;
			break;
		case Gamemode::Classic:
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






TickStatus VoteMenuBehavior::TickTbbf(TbbfMacroInstance* instance, TbbfCustomContext* context, const FrameView& currentFrame) {
	if (context->voteStatus == VoteStatus::Finished)
		return TickStatus::Skipped;
	
	if (context->splashStatus == SplashTextStatus::Welcome) {
		context->voteStatus == VoteStatus::Finished;
		return TickStatus::Yield;
	}
	if (context->splashStatus == SplashTextStatus::Unknown) {
		m_debounceMs = 100;
		return TickStatus::Yield;
	}
	std::optional<POINT> skipVoteButton = FindSkipVote(currentFrame);
	switch (context->voteStatus) {
	case VoteStatus::Loading:
		context->voteStatus = VoteStatus::MapVote;
		m_debounceMs = 5000;
		break;
	case VoteStatus::MapVote:
		VoteBlockyCastle(instance, currentFrame);
		context->voteStatus = VoteStatus::GamemodeVote;
		m_debounceMs = 8000;
		if (skipVoteButton)
			instance->ClickClient(*skipVoteButton);
		else
			m_debounceMs += 20000;
		break;
	case VoteStatus::GamemodeVote:
		VoteGamemode(instance, currentFrame, Gamemode::Classic);
		context->voteStatus = VoteStatus::Waiting;
		m_debounceMs = 5000;
		if (skipVoteButton)
			instance->ClickClient(*skipVoteButton);
		else
			m_debounceMs += 20000;
		break;
	case VoteStatus::Waiting:
		m_debounceMs = 16;
		break;
	}
	return TickStatus::Yield;
}