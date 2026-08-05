#pragma once

#include <utils/WindowUtils.h>
#include "FrameView.h"

enum class PixelCaptureMode {
	GDI = 0,
	WGC = 1
};
enum class PixelCaptureStatus {
	Uninitialized = 0,
	Running = 1,
	Closed = 2
};
class IPixelCapture {
public:
	virtual ~IPixelCapture() = default;
	virtual bool Initialize(HWND targetHwnd = nullptr) = 0;
	virtual void Close() = 0;
	virtual bool CaptureRegion(const Rect& region) = 0;
	virtual bool CaptureClientRegion(const Rect& region) = 0;

	virtual PixelCaptureStatus GetStatus() const = 0;
	virtual FrameView GetFrameView() const = 0;
	virtual int GetWidth() const = 0;
	virtual int GetHeight() const = 0;
};
