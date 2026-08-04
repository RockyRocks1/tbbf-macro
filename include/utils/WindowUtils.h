#pragma once
#include <windows.h>
struct Rect {
	int x = 0;
	int y = 0;
	int width = 0;
	int height = 0;
};
namespace WindowUtils {
    bool IsMainWindow(HWND hwnd);
    HWND FindMainWindow(DWORD processId);
	Rect GetClientRectRelativeToWindow(HWND hwnd);
}