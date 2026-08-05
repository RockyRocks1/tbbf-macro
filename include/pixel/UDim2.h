#include <utils/WindowUtils.h>

struct UDim2 {
    float scaleX = 0.0f;
    int offsetX = 0;
    float scaleY = 0.0f;
    int offsetY = 0;
    constexpr POINT Resolve(const Size2D& bounds) const noexcept {
        const float posX = (static_cast<float>(bounds.width) * scaleX) + static_cast<float>(offsetX);
        const float posY = (static_cast<float>(bounds.height) * scaleY) + static_cast<float>(offsetY);
        return {
             static_cast<int>(posX >= 0.0f ? posX + 0.5f : posX - 0.5f),
             static_cast<int>(posY >= 0.0f ? posY + 0.5f : posY - 0.5f)
        };
    }
};