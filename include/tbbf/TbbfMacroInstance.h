#include <macro/IMacroInstance.h>
#include "TbbfCustomContext.h"
#include "DisconnectBehavior.h"
#include "GameLoadedBehavior.h"
#include "CentralContextBehavior.h"
#include "VoteMenuBehavior.h"
#include "TowerListBehavior.h"
#include "RespawnBehavior.h"
#include "WaveChangedBehavior.h"
#include "TowerSelectBehavior.h"
#include "AttackBehavior.h"

class TbbfMacroInstance : public IMacroInstance {
private:
	std::jthread m_thread;
	std::unique_ptr<RobloxGame> m_game;
	std::unique_ptr<TbbfCustomContext> m_context;
	std::vector<std::unique_ptr<IMacroBehavior>> m_behaviors;
	std::atomic<bool> m_isLoopRunning{ false };
	void PerformMainLoop();
public:
	TbbfMacroInstance() = default;
	~TbbfMacroInstance() override = default;

	bool Initialize(std::unique_ptr<RobloxGame> game) override;
	void SendKey(WORD virtualKey) const;
	void RepeatKey(WORD virtualKey, int repetitions) const;

	void ToggleUiFocus() const;
	void ClickClient(const POINT& clickPosition, uint64_t holdTime = 0) const;

	static uint64_t GetCurrentTimestamp();
};