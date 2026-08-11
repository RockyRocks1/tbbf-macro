#include <tbbf/TowerListBehavior.h>
#include <tbbf/TbbfMacroInstance.h>
#include <iostream>

void TowerListBehavior::CalibrateStart(TbbfMacroInstance* instance) {
    instance->ToggleUiFocus();
    instance->SendKey(VK_DOWN);
    instance->ToggleUiFocus();
    instance->ToggleUiFocus();
    instance->SendKey(VK_UP);
}

bool TowerListBehavior::PassedThicknessCheck(const FrameView& frame, int x, int y, int stepX, int stepY, int targetThickness) {
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
}

std::optional<const Rect> TowerListBehavior::FindNavBoxWithinTowerList(const FrameView& currentFrame) {
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
}
std::string TowerListBehavior::ReadTextWithinNavBox(const FrameView& currentFrame, const Rect& navBox) {
    static constexpr int blueFadePadding = 4;
    static constexpr int luminanceOffset = 50;
    static constexpr UDim2 towerLimitUDim2(0.86f, 0, 0.f, 10);
    static constexpr Size2D towerLimitSize{ 28, 20 };
    int cropX = navBox.x + uiStrokeWidth + blueFadePadding;
    int cropY = navBox.y + uiStrokeWidth + blueFadePadding;
    int cropWidth = navBox.width - ((uiStrokeWidth + blueFadePadding) * 2);
    int cropHeight = navBox.height - ((uiStrokeWidth + blueFadePadding) * 2);

    const FrameView crop = PixelModifier::Crop(currentFrame, cropX, cropY, cropWidth, cropHeight);

    FrameBuffer grayscaleBuffer;
    PixelModifier::Grayscale(crop, grayscaleBuffer);
    const FrameView grayscale = grayscaleBuffer.GetView();

    POINT towerLimitPosition = towerLimitUDim2.Resolve(Size2D{ crop.width, crop.height });
    FrameBuffer censorBuffer;
    PixelModifier::Censor(grayscale, censorBuffer, towerLimitPosition.x - towerLimitSize.width / 2, towerLimitPosition.y - towerLimitSize.height / 2, towerLimitSize.width, towerLimitSize.height);
    const FrameView censor = censorBuffer.GetView();
   
    std::optional<uint8_t> maxLuminance = PixelAnalyzer::GetMaxLuminance(censor);
    if (!maxLuminance)
        return "";
    FrameBuffer thresholdBuffer;
    PixelModifier::Threshold(censor, thresholdBuffer, *maxLuminance - luminanceOffset, ThresholdType::BINARY);
    std::string result = OcrTool::RecognizeText(thresholdBuffer.GetView());
    return result;
}
void TowerListBehavior::ProcessTowerListReading(TbbfMacroInstance* instance, TbbfCustomContext* context, const FrameView& currentFrame) {
    std::optional<const Rect> navBox = FindNavBoxWithinTowerList(currentFrame);
    TowerListInfo& info = context->towerListInfo;
    if (!navBox) {
        info.status = TowerListStatus::Done;
        instance->ToggleUiFocus();
        return;
    }

    instance->SendKey(VK_DOWN);
    std::string towerName = ReadTextWithinNavBox(currentFrame, *navBox);
    if (towerName.find("Enforcer") != std::string::npos) {
        info.enforcerIndex = info.currentReadIndex;
    }
    else if (towerName.find("Void") != std::string::npos || towerName.find("void") != std::string::npos) {
        info.voidTraitorIndex = info.currentReadIndex;
    }
    info.currentReadIndex++;
}

TickStatus TowerListBehavior::TickTbbf(TbbfMacroInstance* instance, TbbfCustomContext* context, const FrameView& currentFrame) {
    if (context->towerListInfo.status == TowerListStatus::Done || context->splashStatus != SplashTextStatus::Welcome)
        return TickStatus::Skipped;
    
    switch (context->towerListInfo.status) {
    case TowerListStatus::Uninitialized:
        context->towerListInfo.status = TowerListStatus::Reading;
        CalibrateStart(instance);
        m_debounceMs = 500;
        return TickStatus::Yield;
    case TowerListStatus::Reading:
        ProcessTowerListReading(instance, context, currentFrame);
        context->playerStatus = PlayerStatus::Menu;
        return TickStatus::Yield;
    default:
        break;
    }

	return TickStatus::Skipped;
}