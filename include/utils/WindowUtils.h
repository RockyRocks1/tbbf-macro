#pragma once
#include <Windows.h>
#include <dwmapi.h>
#include <cmath>
#include <optional>
#include "Rect.h"
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