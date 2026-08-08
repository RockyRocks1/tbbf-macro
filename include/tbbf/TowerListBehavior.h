#pragma once
#include "TbbfMacroBehavior.h"
#include "TbbfCustomContext.h"
#include <pixel/UDim2.h>
#include <ocr/OcrTool.h>

class TbbfMacroInstance;

class TowerListBehavior : public TbbfMacroBehavior {
private:
	static constexpr int uiStrokeWidth = 6;
	bool PassedThicknessCheck(const FrameView& frame, int x, int y, int stepX, int stepY, int targetThickness = uiStrokeWidth);
	std::optional<const Rect> FindNavBoxWithinTowerList(const FrameView& currentFrame);
	std::string ReadTextWithinNavBox(const FrameView& currentFrame, const Rect& navBox);
	void ProcessTowerListReading(TbbfMacroInstance* instance, TbbfCustomContext* context, const FrameView& currentFrame);
	void CalibrateStart(TbbfMacroInstance* instance);
protected:
	TickStatus TickTbbf(TbbfMacroInstance* instance, TbbfCustomContext* context, const FrameView& currentFrame) override;
public:
	TowerListBehavior() : TbbfMacroBehavior(16) {};
};
