#pragma once
#include <tbbf/behaviors/TbbfMacroBehavior.h>
#include <tbbf/context/TbbfContext.h>
#include <tbbf/parsers/ScreenStateReader.h>
#include <tbbf/parsers/DisconnectReader.h>
#include <tbbf/parsers/GameLoadedReader.h>
#include <tbbf/parsers/WaveTextReader.h>
#include <tbbf/parsers/ToolSlotReader.h>
#include <tbbf/parsers/VoteMenuReader.h>

class TbbfMacroInstance;

class ContextBehavior : public TbbfMacroBehavior {
protected:
	TickStatus TickTbbf(TbbfMacroInstance* instance, TbbfContext* context, const FrameView& currentFrame) override;
public:
	ContextBehavior() : TbbfMacroBehavior(16) {};
};
