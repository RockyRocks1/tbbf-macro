#include <macro/IMacroInstance.h>
#include <tbbf/context/TbbfContext.h>
#include <tbbf/behaviors/ContextBehavior.h> 
#include <tbbf/behaviors/DecisionBehavior.h>
#include <tbbf/behaviors/ShutdownBehavior.h>
#include <tbbf/behaviors/VoteMenuBehavior.h>
#include <tbbf/behaviors/TowerListBehavior.h>
#include <tbbf/behaviors/RespawnBehavior.h>
#include <tbbf/behaviors/TowerSelectBehavior.h>
#include <tbbf/behaviors/AttackBehavior.h>
#include <tbbf/behaviors/UpgradeBehavior.h>
#include <tbbf/behaviors/GameMenuBehavior.h>
#include <tbbf/behaviors/ToolBehavior.h>
#include <tbbf/behaviors/DeployBehavior.h>

class TbbfMacroInstance : public IMacroInstance {
private:
	std::unique_ptr<RobloxGame> m_game;
	std::unique_ptr<TbbfContext> m_context;
	std::vector<std::unique_ptr<IMacroBehavior>> m_behaviors;
	std::atomic<bool> m_isRunning{ false };
public:
	TbbfMacroInstance() = default;
	~TbbfMacroInstance() override = default;

	bool Initialize(std::unique_ptr<RobloxGame> game) override;
	void Tick() override;
	inline bool IsRunning() {
		return m_isRunning.load();
	}
	void SendKey(WORD virtualKey) const;
	void RepeatKey(WORD virtualKey, int repetitions) const;

	void ToggleUiFocus() const;
	void ClickClient(const POINT& clickPosition, uint64_t holdTime = 0) const;

	static uint64_t GetCurrentTimestamp();
};