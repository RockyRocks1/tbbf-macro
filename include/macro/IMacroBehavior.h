#include <pixel/FrameView.h>

class IMacroInstance;

class IMacroBehavior {
	virtual ~IMacroBehavior() = default;

	virtual bool OnTick(IMacroInstance* instance, const FrameView& currentFrame) = 0;
};