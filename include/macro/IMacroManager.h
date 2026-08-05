#pragma once
#include "IMacroInstance.h"
#include "MacroInstanceLaunchInfo.h"

class IMacroManager {
public:
    virtual ~IMacroManager() = default;
        
    virtual bool Start() = 0;
    virtual void Stop() = 0;
        
    virtual bool LaunchMacroInstance(const MacroInstanceLaunchInfo& launchInfo) = 0;
    virtual bool TerminateMacroInstance(int instanceIndex) = 0;

    virtual size_t GetActiveInstanceCount() const = 0;

    virtual bool IsRunning() const = 0;
};
