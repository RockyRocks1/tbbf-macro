#pragma once
#include <pixel/UDim2.h>
#include <pixel/PixelAnalyzer.h>
#include <pixel/PixelModifier.h>
#include <ocr/OcrTool.h>

class TowerListReader {
public:
	static constexpr int uiStrokeWidth = 6;
	static std::optional<std::string> IdentifyTower(const FrameView& currentFrame, const std::vector<std::string>& knownTowers) noexcept;
private:
	static std::string ReadTextWithinNavBox(const FrameView& currentFrame, const Rect& navBox) noexcept;
	static std::optional<const Rect> FindNavBoxWithinTowerList(const FrameView& currentFrame) noexcept;
	static bool PassedThicknessCheck(const FrameView& frame, int x, int y, int stepX, int stepY, int targetThickness = uiStrokeWidth) noexcept;
};