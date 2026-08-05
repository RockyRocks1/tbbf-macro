#pragma once
#include <stdint.h>
#include <vector>
#include <memory>

struct ColorRgba;
struct ColorBgra;

struct ColorRgba {
    uint8_t r = 0, g = 0, b = 0, a = 255;
    bool operator==(const ColorRgba&) const = default;
    bool operator==(const ColorBgra& other) const;
    template <typename T>
    inline bool IsCloseTo(const T& other, int variation = 0) const;
};
struct ColorBgra {
    uint8_t b = 0, g = 0, r = 0, a = 255;
    bool operator==(const ColorBgra&) const = default;
    bool operator==(const ColorRgba& other) const { return other == *this; }
};
template <typename T>
inline bool ColorRgba::IsCloseTo(const T& other, int variation) const {
    if (variation == 0)
        return *this == other;

    return std::abs(static_cast<int>(r) - other.r) <= variation &&
        std::abs(static_cast<int>(g) - other.g) <= variation &&
        std::abs(static_cast<int>(b) - other.b) <= variation;
}
inline bool ColorRgba::operator==(const ColorBgra& other) const {
    return r == other.r && g == other.g && b == other.b;
};
enum class BgraChannel : uint8_t {
    B = 0,
    G = 1,
    R = 2,
    A = 3
};
enum class RgbaChannel : uint8_t {
    R = 0,
    G = 1,
    B = 2,
    A = 3
};
enum class PixelFormat : uint8_t {
	Bgra8,
	Gray8
};

struct FrameView {
    std::shared_ptr<const uint8_t[]> data;
	int width = 0;
    int height = 0;
    size_t stride = 0;
	PixelFormat format = PixelFormat::Bgra8;

    inline size_t GetBufferSize() const noexcept {
        return stride * height;
    }
    inline uint8_t GetBytesPerPixel() const noexcept {
        switch (format) {
        case PixelFormat::Bgra8:
            return 4;
        case PixelFormat::Gray8:
            return 1;
        default:
            return 0;
        };
    }
    inline size_t GetRowPadding() const noexcept {
        return stride - static_cast<size_t>(width * GetBytesPerPixel());
    }
};

struct FrameBuffer {
    std::vector<uint8_t> data;
    int width = 0;
    int height = 0;
    size_t stride = 0;
    PixelFormat format = PixelFormat::Bgra8;

    static FrameBuffer FromView(const FrameView& view) {
        const size_t bufferSize = view.GetBufferSize();

        FrameBuffer frameBuffer;
        frameBuffer.width = view.width;
        frameBuffer.height = view.height;
        frameBuffer.stride = view.stride;
        frameBuffer.format = view.format;

        if (view.data && bufferSize > 0)
            frameBuffer.data.assign(view.data.get(), view.data.get() + bufferSize);

        return frameBuffer;
    }
    
    FrameView GetView() const noexcept {
        std::shared_ptr<const uint8_t[]> sharedData(
            data.data(),
            [](const uint8_t*) {}
        );
        return FrameView{ 
            .data = sharedData,
            .width = width, 
            .height = height, 
            .stride = stride, 
            .format = format 
        };
    }
    static inline size_t GetOptimalStride(size_t currentStride, size_t alignment = 64) noexcept {
        return (currentStride + alignment) & ~alignment;
    }
    inline size_t GetBytesPerPixel() const noexcept {
        switch (format) {
        case PixelFormat::Bgra8:
            return 4;
        case PixelFormat::Gray8:
            return 1;
        default:
            return 0;
        };
    }
    inline size_t GetRowPadding() const noexcept {
        return stride - width * GetBytesPerPixel();
    }
};

