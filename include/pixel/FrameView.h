#pragma once
#include <stdint.h>
#include <vector>

struct ColorRgba {
    uint8_t r = 0, g = 0, b = 0, a = 255;
    bool operator==(const ColorRgba&) const = default;
};
struct ColorBgra {
    uint8_t b = 0, g = 0, r = 0, a = 255;
    bool operator==(const ColorBgra&) const = default;
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
	const uint8_t* data = nullptr;
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
            frameBuffer.data.assign(view.data, view.data + bufferSize);

        return frameBuffer;
    }
    
    FrameView GetView() const noexcept {
        return FrameView{ 
            .data = data.data(), 
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

