#pragma once
#include <Windows.h>
#include <dwmapi.h>
#include <cmath>
#include <optional>
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
enum class AspectRatio {
	StandardDefinition = 0,
	Widescreen = 1,
};
namespace WindowUtils {
	bool IsMainWindow(HWND hwnd);
	HWND FindMainWindow(DWORD processId);
	std::optional<POINT> GetClientOffsetFromWindow(HWND hwnd);
	std::optional<POINT> GetClientOffsetFromWgc(HWND hwnd);
	std::optional<POINT> ToAbsoluteCoordinates(const POINT& screenPosition);
	bool SetAspectRatio(HWND hwnd, int targetWidth = 800, AspectRatio ratio = AspectRatio::StandardDefinition);
};