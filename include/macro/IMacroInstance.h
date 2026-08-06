#pragma once
#include "IMacroBehavior.h"

#include <roblox/RobloxGame.h>
#include <pixel/FrameView.h>

class IMacroInstance {
public:
	virtual ~IMacroInstance() = default;

	virtual bool Initialize(std::unique_ptr<RobloxGame> game) = 0;
	
	virtual void* GetCustomContext() = 0;
};