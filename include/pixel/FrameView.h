#pragma once
#include <stdint.h>
#include <vector>
#include <fstream>
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

inline bool SaveFrameViewToBmp(const FrameView& frame, const std::string& filename) {
    if (frame.data == nullptr || frame.width <= 0 || frame.height <= 0) {
        return false;
    }

    // BMP rows must be padded to a multiple of 4 bytes
    size_t targetRowSize = (frame.width * frame.GetBytesPerPixel() + 3) & ~3;
    size_t pixelDataSize = targetRowSize * frame.height;
    size_t fileHeaderSize = 14;
    size_t infoHeaderSize = 40;
    size_t fileSize = fileHeaderSize + infoHeaderSize + pixelDataSize;

    std::ofstream file(filename, std::ios::binary);
    if (!file) return false;

    // 1. BITMAPFILEHEADER (14 bytes)
    uint8_t fileHeader[14] = {
        'B', 'M',                           // Signature
        static_cast<uint8_t>(fileSize),     // File size (bytes 2-5)
        static_cast<uint8_t>(fileSize >> 8),
        static_cast<uint8_t>(fileSize >> 16),
        static_cast<uint8_t>(fileSize >> 24),
        0, 0,                               // Reserved
        0, 0,                               // Reserved
        static_cast<uint8_t>(fileHeaderSize + infoHeaderSize), 0, 0, 0 // Offset to pixel data
    };
    file.write(reinterpret_cast<char*>(fileHeader), sizeof(fileHeader));

    // 2. BITMAPINFOHEADER (40 bytes)
    uint8_t bpp = (frame.format == PixelFormat::Gray8) ? 8 : 32;
    uint32_t imgWidth = frame.width;
    uint32_t imgHeight = frame.height;
    uint32_t imgImageSize = static_cast<uint32_t>(pixelDataSize);

    uint8_t infoHeader[40] = {
        40, 0, 0, 0,                        // Header size
        static_cast<uint8_t>(imgWidth),     // Width
        static_cast<uint8_t>(imgWidth >> 8),
        static_cast<uint8_t>(imgWidth >> 16),
        static_cast<uint8_t>(imgWidth >> 24),
        static_cast<uint8_t>(imgHeight),    // Height (positive = bottom-up)
        static_cast<uint8_t>(imgHeight >> 8),
        static_cast<uint8_t>(imgHeight >> 16),
        static_cast<uint8_t>(imgHeight >> 24),
        1, 0,                               // Planes
        bpp, 0,                             // Bits per pixel
        0, 0, 0, 0,                         // Compression (0 = none)
        static_cast<uint8_t>(imgImageSize), // Image size
        static_cast<uint8_t>(imgImageSize >> 8),
        static_cast<uint8_t>(imgImageSize >> 16),
        static_cast<uint8_t>(imgImageSize >> 24),
        0x13, 0x0B, 0, 0,                   // X pixels per meter (2835)
        0x13, 0x0B, 0, 0,                   // Y pixels per meter (2835)
        0, 0, 0, 0,                         // Colors used
        0, 0, 0, 0                          // Important colors
    };
    file.write(reinterpret_cast<char*>(infoHeader), sizeof(infoHeader));

    // Grayscale palette (if Gray8, write 256 grayscale entries)
    if (frame.format == PixelFormat::Gray8) {
        for (int i = 0; i < 256; ++i) {
            uint8_t paletteEntry[4] = { static_cast<uint8_t>(i), static_cast<uint8_t>(i), static_cast<uint8_t>(i), 0 };
            file.write(reinterpret_cast<char*>(paletteEntry), 4);
        }
    }

    // 3. Pixel Data (BMP expects bottom-up row order)
    std::vector<uint8_t> paddingBuffer(targetRowSize, 0);
    const uint8_t* srcPixels = frame.data.get();

    for (int y = frame.height - 1; y >= 0; --y) {
        const uint8_t* srcRow = srcPixels + (y * frame.stride);
        file.write(reinterpret_cast<const char*>(srcRow), frame.width * frame.GetBytesPerPixel());

        // Write row padding bytes if necessary
        size_t currentPadding = targetRowSize - (frame.width * frame.GetBytesPerPixel());
        if (currentPadding > 0) {
            file.write(reinterpret_cast<char*>(paddingBuffer.data()), currentPadding);
        }
    }

    return true;
}