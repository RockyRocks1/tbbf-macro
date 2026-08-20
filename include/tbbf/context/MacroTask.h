#pragma once
enum class TbbfMacroTask {
	Idle,

	Shutdown,
	Game_WaitForLoad,
	
	Vote_WaitForLoad,
	Vote_SubmittingMap,
	Vote_SubmittingGamemode,
	Vote_WaitingForMatch,

	TowerList_Initializing,
	TowerList_ReadingItem,

	TowerSelect_Selecting,
	TowerSelect_Buying,
	TowerSelect_Equipping,

	Respawn_GoToMenu,
	Respawn_Spawning,

	Combat
};