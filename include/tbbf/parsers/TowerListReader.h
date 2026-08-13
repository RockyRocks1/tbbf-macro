#pragma once
#include <pixel/UDim2.h>
#include <pixel/FrameView.h>

class TowerListReader {
public:
	static constexpr int uiStrokeWidth = 6;

	static std::string ReadTextWithinNavBox(const FrameView& currentFrame, const Rect& navBox) noexcept;
	static std::optional<const Rect> FindNavBoxWithinTowerList(const FrameView& currentFrame) noexcept;
private:
	static bool PassedThicknessCheck(const FrameView& frame, int x, int y, int stepX, int stepY, int targetThickness = uiStrokeWidth) noexcept;
};