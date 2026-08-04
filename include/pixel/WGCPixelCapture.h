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
#include <utils/WindowUtils.h>

class WgcPixelCapture : public IPixelCapture {
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
	uint32_t m_width = 0;
	uint32_t m_height = 0;

	void OnFrameArrived(const winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool& sender, const winrt::Windows::Foundation::IInspectable& args);
	bool EnsureStagingTexture(uint32_t width, uint32_t height);
public:
	WgcPixelCapture() = default;
	~WgcPixelCapture() override;

	bool Initialize(HWND targetHwnd = nullptr) override;
	bool CaptureRegion(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

	inline FrameView GetFrameView() const noexcept override {
		return {
			.data = static_cast<const uint8_t*>(m_pBuffer),
			.width = m_width,
			.height = m_height,
			.stride = m_width * 4,
			.format = PixelFormat::Bgra8
		};
	};
	inline uint32_t GetWidth() const noexcept override {
		return m_width;
	};
	inline uint32_t GetHeight() const noexcept override {
		return m_height;
	};
};