#include "IMacroBehavior.h"

#include <roblox/RobloxGame.h>
#include <pixel/FrameView.h>

class IMacroInstance {
	virtual ~IMacroInstance() = default;

	virtual bool Initialize(RobloxGame* game) = 0;
	virtual bool Tick() = 0;
	virtual const FrameView& GetLatestFrame() const = 0;
	virtual const Size2D& GetClientBounds() const = 0;
	virtual bool WasDisconnected() const = 0;

	virtual void* GetCustomContext() = 0;
};