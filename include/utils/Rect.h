#pragma once

struct Rect {
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;
};
struct Size2D {
    int width = 0;
    int height = 0;

    inline bool operator==(const Size2D& other) const {
        return width == other.width && height == other.height;
    }
};