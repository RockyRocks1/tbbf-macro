#include <macro/IMacroInstance.h>
#include "DisconnectBehavior.h"
#include "GameLoadedBehavior.h"

struct TbbfCustomContext {
	int enforcerTowerIndex;
	int voidTraitorTowerIndex;
};
class TbbfMacroInstance : public IMacroInstance {
private:
	std::jthread m_thread;
	TbbfCustomContext m_context{};
	std::unique_ptr<RobloxGame> m_game;
	std::vector<std::unique_ptr<IMacroBehavior>> m_behaviors;
	std::atomic<bool> m_isLoopRunning{ false };
	void PerformMainLoop();
public:
	TbbfMacroInstance() = default;
	~TbbfMacroInstance() override = default;

	bool Initialize(std::unique_ptr<RobloxGame> game) override;
	inline void* GetCustomContext() override {
		return static_cast<void*>(&m_context);
	}
	
	/*

	POINT GetWelcomePosition();
	POINT GetInGameMenuPosition();
	POINT GetHealthBarPosition();

	bool IsInMainMenu(const FrameView& currentFrame);
	bool IsInSelectSpawnMenu(const FrameView& currentFrame);
	bool IsDeployed(const FrameView& currentFrame);
	bool HasTool(const FrameView& currentFrame);
	bool IsGameOver(const FrameView& currentFrame);
	bool IsTriumph(const FrameView& currentFrame);
	*/
};