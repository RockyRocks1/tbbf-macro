#pragma once
#include <pixel/UDim2.h>
#include <pixel/PixelAnalyzer.h>
#include <pixel/PixelModifier.h>

class ToolSlotReader {
public:
    static bool IsSlotExists(const FrameView& currentFrame) noexcept;
    static bool IsSlotActive(const FrameView& currentFrame) noexcept;

private:
    static const Rect GetSlotRect(const FrameView& currentFrame) noexcept;
    static constexpr UDim2 slotOneUDim{ 0.5f, 0, 1.0f, -35 };
    static constexpr Size2D slotSize{ 60, 60 };
};
