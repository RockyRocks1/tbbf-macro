#pragma once

#include <winrt/windows.foundation.h>
#include <winrt/windows.graphics.capture.h>
#include <winrt/windows.graphics.directx.direct3d11.h>
#include <windows.graphics.capture.interop.h>
#include <windows.graphics.directx.direct3d11.interop.h>
#include <wil/cppwinrt.h>
#include <wil/com.h>
#include <d3d11.h>
#include <mutex>
#include "IPixelCapture.h"

class WGCPixelCapture : public IPixelCapture {
private:
	HWND m_targetHwnd = nullptr;

	// the block below is initialized once
	wil::com_ptr<ID3D11Device> m_device;
	wil::com_ptr<ID3D11DeviceContext> m_context;
	winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice m_direct3DDevice{ nullptr };
	winrt::Windows::Graphics::Capture::GraphicsCaptureItem m_item{ nullptr };
	winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool m_framePool{ nullptr };
	winrt::Windows::Graphics::SizeInt32 m_currentFramePoolSize;
	winrt::event_token m_frameArrivedToken{};
	winrt::Windows::Graphics::Capture::GraphicsCaptureSession m_captureSession{ nullptr };

	wil::com_ptr<ID3D11Texture2D> m_currentSourceTexture;
	wil::com_ptr<ID3D11Texture2D> m_stagingTexture;
	std::mutex m_frameMutex;

	void* m_pBuffer = nullptr;
	size_t m_bufferCapacity = 0;
	int m_width = 0;
	int m_height = 0;

	void OnFrameArrived(const winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool& sender, const winrt::Windows::Foundation::IInspectable& args);
	bool EnsureStagingTexture(int width, int height);
public:
	WGCPixelCapture() = default;
	~WGCPixelCapture() override;

	bool Initialize(HWND targetHwnd = nullptr) override;
	bool CaptureRegion(int x, int y, int width, int height) override;

	ColorRGBA GetPixel(int relX, int relY) const override;
	const uint8_t* GetBuffer() const override;
	int GetWidth() const override;
	int GetHeight() const override;
};