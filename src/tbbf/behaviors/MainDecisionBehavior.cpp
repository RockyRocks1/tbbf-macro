#pragma once
#include <tbbf/behaviors/MainDecisionBehavior.h>
#include <tbbf/TbbfMacroInstance.h>

TickStatus MainDecisionBehavior::TickTbbf(TbbfMacroInstance* instance, TbbfContext* context, const FrameView& currentFrame) {
    uint64_t currentTimestamp = instance->GetCurrentTimestamp();

    TbbfContext::TbbfMacroDecisions& decisions = context->decisions;
    TbbfContext::TimestampInfo& timestampInfo = context->timestamps;
   

    if (context->isDisconnected) {
        decisions.currentTask = TbbfMacroTask::Shutdown;
        return TickStatus::Skipped;
    }
 
    if (!context->isGameLoaded) {
        decisions.currentTask = TbbfMacroTask::Game_WaitForLoad;
        return TickStatus::Skipped;
    }
    else if (decisions.currentTask == TbbfMacroTask::Game_WaitForLoad)
        decisions.currentTask = TbbfMacroTask::Idle;

    TbbfMacroTask lastTask = decisions.currentTask;

    decisions.shouldAttack = false;
    decisions.shouldUpgrade = false;
    decisions.shouldEquipTool = false;
    decisions.shouldOpenMenu = false;
    decisions.shouldSpawn = true;

    if (lastTask == TbbfMacroTask::Idle) {
        decisions.lastExecutedTask = TbbfMacroTask::Idle;
    }
    
    switch (lastTask) {
    case TbbfMacroTask::Vote_WaitForLoad:
        if (currentTimestamp >= timestampInfo.nextVoteActionTick) {
            decisions.currentTask = TbbfMacroTask::Vote_SubmittingMap;
            timestampInfo.nextVoteActionTick = context->uiLayout.skipVoteButtonPos.has_value()
                ? currentTimestamp + 5000
                : currentTimestamp + 25000;
        }
        break;
    case TbbfMacroTask::Vote_SubmittingMap:
        if (currentTimestamp >= timestampInfo.nextVoteActionTick) {
            decisions.currentTask = TbbfMacroTask::Vote_SubmittingGamemode;

            timestampInfo.nextVoteActionTick = context->uiLayout.skipVoteButtonPos.has_value()
                ? currentTimestamp + 5000
                : currentTimestamp + 25000;
        }
        break;
    case TbbfMacroTask::Vote_SubmittingGamemode:
        if (currentTimestamp >= timestampInfo.nextVoteActionTick)
            decisions.currentTask = TbbfMacroTask::Vote_WaitingForMatch;
        break;
    case TbbfMacroTask::Vote_WaitingForMatch:
        if (context->splashStatus == SplashTextStatus::Welcome)
            decisions.currentTask = TbbfMacroTask::Idle;
        break;

    case TbbfMacroTask::TowerList_Initializing:
        decisions.currentTask = TbbfMacroTask::TowerList_ReadingItem;
        break;
    case TbbfMacroTask::TowerList_ReadingItem:
        if (context->towerList.status == TowerListStatus::Ready) 
            decisions.currentTask = TbbfMacroTask::Idle;
        break;

       
    case TbbfMacroTask::TowerSelect_Selecting:
        if (decisions.lastExecutedTask == TbbfMacroTask::TowerSelect_Selecting)
            decisions.currentTask = TbbfMacroTask::TowerSelect_Buying;
        break;
    case TbbfMacroTask::TowerSelect_Buying:
        if (decisions.lastExecutedTask == TbbfMacroTask::TowerSelect_Buying)
            decisions.currentTask = TbbfMacroTask::TowerSelect_Equipping;
        break;
    case TbbfMacroTask::TowerSelect_Equipping:
        if (decisions.lastExecutedTask == TbbfMacroTask::TowerSelect_Equipping)
            decisions.currentTask = TbbfMacroTask::Respawn_Spawning;
        break;

    case TbbfMacroTask::Respawn_GoToMenu:
        if (decisions.lastExecutedTask == TbbfMacroTask::Respawn_GoToMenu)
            decisions.currentTask = TbbfMacroTask::Idle;
        break;
    case TbbfMacroTask::Respawn_Spawning:
        if (context->playerStatus == PlayerStatus::Deployed)
            decisions.currentTask = TbbfMacroTask::Combat;
        break;

    default:
        if (context->playerStatus == PlayerStatus::Dead) {
            decisions.currentTask = TbbfMacroTask::Respawn_GoToMenu;
            break;
        }

        if (context->splashStatus == SplashTextStatus::Vote) {
            decisions.currentTask = TbbfMacroTask::Vote_WaitForLoad;
            timestampInfo.nextVoteActionTick = currentTimestamp + 5000;
            break;
        }

        if (context->towerList.status == TowerListStatus::Pending && context->playerStatus == PlayerStatus::Menu) {
            decisions.currentTask = TbbfMacroTask::TowerList_Initializing;
            break;
        }

        if (context->playerStatus == PlayerStatus::Menu &&
            (context->waveNumber == 0 || context->waveNumber == 14) &&
            context->lastProcessedWave != context->waveNumber &&
            context->towerList.status == TowerListStatus::Ready)
        {
            decisions.currentTask = TbbfMacroTask::TowerSelect_Selecting;
            break;
        }

        if (context->playerStatus == PlayerStatus::Menu && decisions.shouldSpawn) {
            decisions.currentTask = TbbfMacroTask::Respawn_Spawning;
            break;
        }

        break;
    }

    if (decisions.currentTask == TbbfMacroTask::Combat) {
        decisions.shouldAttack = true;
        if (!context->isMiniMenuActive)
            decisions.shouldOpenMenu = true;

        if (context->tool.exists && !context->tool.isActive)
            decisions.shouldEquipTool = true;
    }

    if (decisions.currentTask == TbbfMacroTask::Respawn_Spawning)
    {
        if (context->playerStatus == PlayerStatus::Menu)
            decisions.shouldUpgrade = true;
    }

    return TickStatus::Skipped;
}
