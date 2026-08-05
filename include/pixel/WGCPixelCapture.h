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
#include <atomic>
#include <utils/WindowUtils.h>

class WgcPixelCapture : public IPixelCapture {
private:
	HWND m_targetHwnd = nullptr;

	wil::com_ptr<ID3D11Device> m_device;
	wil::com_ptr<ID3D11DeviceContext> m_context;
	winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice m_direct3DDevice{ nullptr };
	winrt::Windows::Graphics::Capture::GraphicsCaptureItem m_item{ nullptr };
	winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool m_framePool{ nullptr };
	winrt::Windows::Graphics::SizeInt32 m_currentFramePoolSize;
	winrt::event_token m_frameArrivedToken{};
	winrt::Windows::Graphics::Capture::GraphicsCaptureSession m_captureSession{ nullptr };

	wil::com_ptr<ID3D11Texture2D> m_stagingTexture;
	mutable std::mutex m_frameMutex;

	FrameView m_latestFrame;
	std::atomic<PixelCaptureStatus> m_status{ PixelCaptureStatus::Uninitialized };

	void OnFrameArrived(const winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool& sender, const winrt::Windows::Foundation::IInspectable& args);
	bool EnsureStagingTexture(int width, int height);
	bool EnsureFramePool(winrt::Windows::Graphics::Capture::Direct3D11CaptureFrame frame);
public:
	WgcPixelCapture() = default;
	~WgcPixelCapture() override;

	bool Initialize(HWND targetHwnd = nullptr) override;
	void Close() override;

	inline PixelCaptureStatus GetStatus() const noexcept override {
		return m_status.load();
	}
	inline FrameView GetLatestFrame() const override {
		std::lock_guard<std::mutex> lock(m_frameMutex);
		return m_latestFrame;
	};
};