#pragma once
#include <stdint.h>
#include <vector>

struct ColorRgba {
    uint8_t r = 0, g = 0, b = 0, a = 255;
};
struct ColorBgra {
    uint8_t b = 0, g = 0, r = 0, a = 255;
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
	uint32_t width = 0;
    uint32_t height = 0;
    uint32_t stride = 0;
	PixelFormat format = PixelFormat::Bgra8;

    inline uint32_t GetBufferSize() const noexcept {
        return stride * height;
    }
    inline uint32_t GetBytesPerPixel() const noexcept {
        switch (format) {
        case PixelFormat::Bgra8:
            return 4;
        case PixelFormat::Gray8:
            return 1;
        default:
            return 0;
        };
    }
    inline uint32_t GetRowPadding() const noexcept {
        return stride - width * GetBytesPerPixel();
    }
};

struct FrameBuffer {
    std::vector<uint8_t> data;
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t stride = 0;
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
    static inline uint32_t GetOptimalStride(uint32_t currentStride, uint8_t alignment = 64) noexcept {
        return (currentStride + alignment) & ~alignment;
    }
    inline uint32_t GetBytesPerPixel() const noexcept {
        switch (format) {
        case PixelFormat::Bgra8:
            return 4;
        case PixelFormat::Gray8:
            return 1;
        default:
            return 0;
        };
    }
    inline uint32_t GetRowPadding() const noexcept {
        return stride - width * GetBytesPerPixel();
    }
};

