#include <tbbf/behaviors/CentralContextBehavior.h>
#include <tbbf/TbbfMacroInstance.h>

TickStatus CentralContextBehavior::TickTbbf(TbbfMacroInstance* instance, TbbfContext* context, const FrameView& currentFrame) {

	uint64_t currentTimestamp = instance->GetCurrentTimestamp();
	context->isDisconnected = DisconnectReader::IsDisconnected(currentFrame);
	if (context->isDisconnected)
		return TickStatus::Skipped;
	context->isGameLoaded = GameLoadedReader::IsGameLoaded(currentFrame);

	TbbfContext::UiLayoutInfo& layoutInfo = context->uiLayout;
	TbbfContext::TimestampInfo& timestampInfo = context->timestamps;

	if (context->uiLayout.HasResized(currentFrame)) {
		layoutInfo.viewportSize = Size2D{ currentFrame.width, currentFrame.height };
		layoutInfo.inGameMenuOpenOffset = 225;

		layoutInfo.inGameMenuAnchor = ScreenStateReader::GetInGameMenuAnchor(currentFrame);
		layoutInfo.healthBarBounds = ScreenStateReader::GetHealthBarBounds(currentFrame);
		layoutInfo.expBarBounds = ScreenStateReader::GetExpBarBounds(currentFrame, layoutInfo.inGameMenuAnchor);
		layoutInfo.waveTextBounds = ScreenStateReader::GetWaveTextBounds(currentFrame, layoutInfo.inGameMenuAnchor);

		layoutInfo.skipVoteButtonPos = std::nullopt;
	}
	context->splashStatus = ScreenStateReader::GetSplashTextStatus(currentFrame);
	if (context->splashStatus == SplashTextStatus::Vote) {
		if (!layoutInfo.skipVoteButtonPos)
			layoutInfo.skipVoteButtonPos = VoteMenuReader::GetSkipVotePosition(currentFrame);
	} else 
		layoutInfo.skipVoteButtonPos = std::nullopt;

	PlayerStatus oldPlayerStatus = context->playerStatus;
	uint64_t msSinceRespawn = currentTimestamp - context->timestamps.lastRespawnTick;
	context->isInvincible = (msSinceRespawn < 4000);
	context->playerStatus = ScreenStateReader::GetPlayerStatus(oldPlayerStatus, context->splashStatus, layoutInfo, context->isInvincible, currentFrame);
	if (context->playerStatus == PlayerStatus::Deployed && context->playerStatus != oldPlayerStatus)
		timestampInfo.lastRespawnTick = currentTimestamp;

	context->isMiniMenuActive = ScreenStateReader::IsInGameMenuOpen(currentFrame, layoutInfo.expBarBounds);
	static constexpr int waveReaderCooldown = 5000;
	if (currentTimestamp - timestampInfo.lastWaveReadTick >= waveReaderCooldown && context->playerStatus == PlayerStatus::Deployed) {
		const int lastWaveNumber = context->waveNumber;

		std::optional<int> currentWaveNumber = WaveTextReader::ReadFromFrame(currentFrame, layoutInfo.waveTextBounds);
		if (currentWaveNumber && *currentWaveNumber != lastWaveNumber) {
			context->waveNumber = *currentWaveNumber;
			timestampInfo.lastWaveChangedTick = currentTimestamp;
		}
		timestampInfo.lastWaveReadTick = currentTimestamp;
	}

	context->tool.exists = (context->playerStatus == PlayerStatus::Deployed) ? ToolSlotReader::IsSlotExists(currentFrame) : false;
	context->tool.isActive = (context->tool.exists) ? ToolSlotReader::IsSlotActive(currentFrame) : false;
	

	return TickStatus::Skipped;
}