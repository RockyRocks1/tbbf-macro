#pragma once
#include "IMacroBehavior.h"

#include <roblox/RobloxGame.h>
#include <pixel/FrameView.h>

class IMacroInstance {
public:
	virtual ~IMacroInstance() = default;

	virtual bool Initialize(std::unique_ptr<RobloxGame> game) = 0;
	virtual void Tick() = 0;
	virtual bool IsRunning() = 0;

	virtual void SendKey(WORD virtualKey) const = 0;
	virtual void RepeatKey(WORD virtualKey, int repetitions) const = 0;
	virtual void ToggleUiFocus() const = 0;
	virtual void ClickClient(const POINT& clickPosition, uint64_t holdTime) const = 0;

};