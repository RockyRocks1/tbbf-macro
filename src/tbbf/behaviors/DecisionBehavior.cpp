#include <tbbf/behaviors/DecisionBehavior.h>
#include <tbbf/TbbfMacroInstance.h>
TickStatus DecisionBehavior::TickTbbf(TbbfMacroInstance* instance, TbbfContext* context, const FrameView& currentFrame) {
    uint64_t currentTimestamp = instance->GetCurrentTimestamp();
    TbbfContext::Decisions& decisions = context->decisions;
    TbbfContext::TimestampInfo& timestampInfo = context->timestamps;
    TbbfContext::BehaviorStatuses& statuses = context->statuses;

    decisions.commandShutdown = false;
    decisions.commandVoteMenu = false;
    decisions.commandGoToMenu = false;
    decisions.commandReadTowerList = false;
    decisions.commandSelectTower = false;

    decisions.shouldAttack = false;
    decisions.shouldUpgrade = false;
    decisions.shouldEquipTool = false;
    decisions.shouldOpenMenu = false;
    decisions.shouldDeploy = false;
    
    if (context->isDisconnected) {
        decisions.commandShutdown = true;
        return TickStatus::Skipped;
    }
    if (!context->isGameLoaded)
        return TickStatus::Yield;

    if (decisions.isWaitingForMenu) {
        if (context->playerStatus == PlayerStatus::Menu)
            decisions.isWaitingForMenu = false;
        else
            return TickStatus::Skipped;
    }
    
    

    if (statuses.voteMenu != VoteMenuStatus::Idle || context->splashStatus == SplashTextStatus::Vote) {
        decisions.commandVoteMenu = true;
        return TickStatus::Skipped;
    }
    bool isWaveOfInterest = (context->waveNumber == 0 || context->waveNumber == 14) &&
        context->lastProcessedWave != context->waveNumber;

    if (context->playerStatus == PlayerStatus::Menu) {
        if (context->waveNumber == -1) { // Deploy to get the true wave number...
            decisions.shouldDeploy = true;
            return TickStatus::Skipped;
        }
        if (statuses.towerList != TowerListStatus::Ready) {
            decisions.commandReadTowerList = true;
            return TickStatus::Skipped;
        }

        
        if (isWaveOfInterest || statuses.towerSelect != TowerSelectStatus::Idle) {
            decisions.commandSelectTower = true;
            return TickStatus::Skipped;
        }
        
        decisions.shouldUpgrade = true;
        decisions.shouldDeploy = true;
        return TickStatus::Skipped;
    }
    if (context->playerStatus == PlayerStatus::Deployed) {
        if (isWaveOfInterest && context->isMiniMenuActive) {
            decisions.commandGoToMenu = true;
            decisions.isWaitingForMenu = true;
            return TickStatus::Skipped;
        }
        decisions.shouldAttack = true;
       
        if (!context->isMiniMenuActive)
            decisions.shouldOpenMenu = true;

        if (context->tool.exists && !context->tool.isActive)
            decisions.shouldEquipTool = true;
    }
    if (context->playerStatus == PlayerStatus::Dead) {
        decisions.commandGoToMenu = true;
        decisions.shouldDeploy = true;
        return TickStatus::Skipped;
    }
    return TickStatus::Skipped;
}
