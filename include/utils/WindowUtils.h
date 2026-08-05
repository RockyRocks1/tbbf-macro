#pragma once
#include <Windows.h>
#include <dwmapi.h>
#include <optional>
struct Rect {
	int x = 0;
	int y = 0;
	int width = 0;
	int height = 0;
};
namespace WindowUtils {
    bool IsMainWindow(HWND hwnd);
    HWND FindMainWindow(DWORD processId);
	std::optional<POINT> GetClientOffsetFromWindow(HWND hwnd);
	std::optional<POINT> GetClientOffsetFromWgc(HWND hwnd);
}