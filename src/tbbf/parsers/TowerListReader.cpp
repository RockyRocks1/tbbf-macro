#include <tbbf/parsers/TowerListReader.h>

bool TowerListReader::PassedThicknessCheck(const FrameView& frame, int x, int y, int stepX, int stepY, int targetThickness) noexcept {
    static constexpr ColorRgba WHITE{ 255, 255, 255 };

    if (stepX == 0 && stepY == 0)
        return false;
    int currentThickness = 0;
    int currentX = x;
    int currentY = y;
    while (true) {
        std::optional<ColorRgba> pixel = PixelAnalyzer::GetPixelColor(frame, POINT{ currentX, currentY });
        if (!pixel)
            return false;
        if (pixel != WHITE)
            break;
        currentThickness++;
        currentX += stepX;
        currentY += stepY;
        if (currentThickness > targetThickness)
            break;
    }
    return currentThickness == targetThickness;
};
std::optional<const Rect> TowerListReader::FindNavBoxWithinTowerList(const FrameView& currentFrame) noexcept {
    static constexpr Size2D maxSize{ 200, 95 };
    static constexpr Size2D minSize{ 120, 80 };

    const int classlistMaxX = static_cast<int>(currentFrame.width * 0.25f);

    for (int y = 60; y < currentFrame.height - minSize.height; y++) {
        for (int x = 10; x < classlistMaxX; x++) {
            if (!PassedThicknessCheck(currentFrame, x, y, 1, 0))
                continue;

            int topLeftX = x;
            int topLeftY = y - uiStrokeWidth;

            int bottomRightX = topLeftX + minSize.width - uiStrokeWidth;
            bool rightBorderFound = false;

            while (bottomRightX + uiStrokeWidth < currentFrame.width) {
                if (PassedThicknessCheck(currentFrame, bottomRightX, y, 1, 0)) {
                    rightBorderFound = true;
                    break;
                }
                bottomRightX++;
            }

            if (!rightBorderFound)
                continue;

            int computedWidth = (bottomRightX + uiStrokeWidth) - topLeftX;
            if (computedWidth > maxSize.width)
                continue;

            int bottomRightY = topLeftY + minSize.height - uiStrokeWidth;
            bool bottomBorderFound = false;

            while (bottomRightY + uiStrokeWidth < currentFrame.height) {
                if (PassedThicknessCheck(currentFrame, topLeftX + uiStrokeWidth + 1, bottomRightY, 0, 1)) {
                    bottomBorderFound = true;
                    break;
                }
                bottomRightY++;
            }

            if (!bottomBorderFound)
                continue;

            int computedHeight = (bottomRightY + uiStrokeWidth) - topLeftY;
            if (computedHeight > maxSize.height)
                continue;

            return Rect{ topLeftX, topLeftY, computedWidth, computedHeight };
        }
    }
    return std::nullopt;
};
std::string TowerListReader::ReadTextWithinNavBox(const FrameView& currentFrame, const Rect& navBox) noexcept {
    static constexpr int blueFadePadding = 4;
    static constexpr UDim2 towerLimitUDim2(0.86f, 0, 0.f, 10);
    static constexpr Size2D towerLimitSize{ 26, 16 };
    const Rect cropRegion = {
        navBox.x + uiStrokeWidth + blueFadePadding,
        navBox.y + uiStrokeWidth + blueFadePadding,
        navBox.width - ((uiStrokeWidth + blueFadePadding) * 2),
        navBox.height - ((uiStrokeWidth + blueFadePadding) * 2)
    };
    const POINT towerLimitPosition = towerLimitUDim2.Resolve(Size2D{ cropRegion.width, cropRegion.height });
    const Rect towerLimitRegion{
        towerLimitPosition.x - towerLimitSize.width / 2,
        towerLimitPosition.y - towerLimitSize.height / 2,
        towerLimitSize.width,
        towerLimitSize.height
    };
    const FrameView crop = PixelModifier::Crop(currentFrame, cropRegion);
    FrameBuffer grayscaleBuffer;
    PixelModifier::Grayscale(crop, grayscaleBuffer);
    const FrameView grayscale = grayscaleBuffer.GetView();
    FrameBuffer censorBuffer;
    PixelModifier::Censor(grayscale, censorBuffer, towerLimitRegion);
    const FrameView censor = censorBuffer.GetView();
    FrameBuffer normalizeBuffer;
    PixelModifier::Normalize(censor, normalizeBuffer, towerLimitRegion);
    const FrameView normalize = normalizeBuffer.GetView();
    FrameBuffer upscaleBuffer;
    PixelModifier::Upscale(normalize, upscaleBuffer, 2);
    std::string result = OcrTool::RecognizeText(upscaleBuffer.GetView());
    return result;
};
std::optional<std::string> TowerListReader::IdentifyTower(const FrameView& currentFrame,const std::vector<std::string>& knownTowers) noexcept {
    std::optional<const Rect> navBoxOpt = FindNavBoxWithinTowerList(currentFrame);
    if (!navBoxOpt)
        return std::nullopt;

    std::string rawText = ReadTextWithinNavBox(currentFrame, *navBoxOpt);
    if (rawText.empty())
        return "";

    return OcrTool::FindClosestMatch(rawText, knownTowers);
}