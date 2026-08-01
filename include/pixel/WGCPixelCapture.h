#pragma once

#include <winrt/windows.graphics.capture.h>
#include <winrt/windows.graphics.directx.direct3d11.h>
#include <windows.graphics.directx.direct3d11.interop.h>
#include <wil/cppwinrt.h>
#include <wil/com.h>
#include "IPixelCapture.h"

class WGCPixelCapture : public IPixelCapture {
private:
	HWND m_targetHwnd = nullptr;

	wil::com_ptr<ID3D11Device> m_device;
	wil::com_ptr<ID3D11DeviceContext> m_context;
	wil::com_ptr<ID3D11Texture2D> m_stagingTexture;

	winrt::Windows::Graphics::Capture::GraphicsCaptureItem m_item{ nullptr };
	winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool m_framePool{ nullptr };
	winrt::Windows::Graphics::Capture::GraphicsCaptureSession m_captureSession{ nullptr };

	std::vector<uint8_t> m_buffer;
	int m_width = 0;
	int m_height = 0;
	int m_rowPitch = 0;

public:
	WGCPixelCapture() = default;
	~WGCPixelCapture() override;

	bool Initialize(HWND targetHwnd = nullptr) override;
	bool CaptureRegion(int x, int y, int width, int height) override;

	Color GetPixel(int relX, int relY) const override;
	const uint8_t* GetBuffer() const override;
	int GetWidth() const override;
	int GetHeight() const override;
};