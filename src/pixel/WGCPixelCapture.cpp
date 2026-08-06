#include <pixel/WgcPixelCapture.h>



bool WgcPixelCapture::Initialize(HWND targetHwnd) {
	if (m_status.load() != PixelCaptureStatus::Uninitialized)
		return false;
	m_targetHwnd = targetHwnd;
	HRESULT result = D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		D3D11_CREATE_DEVICE_BGRA_SUPPORT,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		m_device.put(),
		nullptr,
		m_context.put()
	);
	if (FAILED(result) || !m_device || !m_context)
		return false;
	// clean this up later
	wil::com_ptr<IDXGIDevice> dxgiDevice = m_device.try_query<IDXGIDevice>();
	if (!dxgiDevice)
		return false;

	winrt::com_ptr<IInspectable> inspectableDevice;
	result = CreateDirect3D11DeviceFromDXGIDevice(dxgiDevice.get(), inspectableDevice.put());
	if (FAILED(result) || !inspectableDevice)
		return false;

	m_direct3DDevice = inspectableDevice.as<winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice>();

	auto interopFactory = winrt::get_activation_factory<winrt::Windows::Graphics::Capture::GraphicsCaptureItem, IGraphicsCaptureItemInterop>();
	result = interopFactory->CreateForWindow(
		m_targetHwnd,
		winrt::guid_of<winrt::Windows::Graphics::Capture::GraphicsCaptureItem>(),
		winrt::put_abi(m_item)
	);
	if (FAILED(result) || !m_item)
		return false;
	m_currentFramePoolSize = m_item.Size();
	m_framePool = winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool::CreateFreeThreaded(
		m_direct3DDevice,
		winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized, 
		2, 
		m_currentFramePoolSize
	);
	
	if (!m_framePool)
		return false;
	m_frameArrivedToken = m_framePool.FrameArrived({ this, &WgcPixelCapture::OnFrameArrived });

	m_captureSession = m_framePool.CreateCaptureSession(m_item);
	if (!m_captureSession)
		return false;

	m_captureSession.IsCursorCaptureEnabled(false);
	m_captureSession.IsBorderRequired(false);
	m_captureSession.StartCapture();
	m_status.store(PixelCaptureStatus::Running);
	return true;
}
void WgcPixelCapture::Close() {
	
};
void WgcPixelCapture::OnFrameArrived(const winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool& sender, const winrt::Windows::Foundation::IInspectable& args) {
	if (m_status.load() != PixelCaptureStatus::Running)
		return;

	winrt::Windows::Graphics::Capture::Direct3D11CaptureFrame frame = sender.TryGetNextFrame();
	if (!EnsureFramePool(frame))
		return;

	const int frameWidth = m_currentFramePoolSize.Width;
	const int frameHeight = m_currentFramePoolSize.Height;
	auto access = frame.Surface().as<Windows::Graphics::DirectX::Direct3D11::IDirect3DDxgiInterfaceAccess>();
	wil::com_ptr<ID3D11Texture2D> newSourceTexture;
	HRESULT	hrInterface = access->GetInterface(IID_PPV_ARGS(newSourceTexture.put()));

	if (FAILED(hrInterface))
		return;
	
	if (!EnsureStagingTexture(frameWidth, frameHeight))
		return;

	m_context->CopyResource(
		m_stagingTexture.get(),
		newSourceTexture.get()
	);

	D3D11_MAPPED_SUBRESOURCE mapped{};
	HRESULT hrMap = m_context->Map(m_stagingTexture.get(), 0, D3D11_MAP::D3D11_MAP_READ, 0, &mapped);
	if (FAILED(hrMap))
		return;
	const size_t stride = static_cast<size_t>(frameWidth) * 4;
	const size_t requiredBytes = stride * frameHeight;

	void* newBuffer = std::malloc(requiredBytes);
		
	if (!newBuffer) {
		m_context->Unmap(m_stagingTexture.get(), 0);
		return;
	}

	const uint8_t* sourceBytes = static_cast<const uint8_t*>(mapped.pData);
	uint8_t* destStart = static_cast<uint8_t*>(newBuffer);
	uint8_t* destIter = destStart;

	for (int row = 0; row < frameHeight; row++) {
		std::memcpy(destIter, sourceBytes, stride);
		destIter += stride;
		sourceBytes += mapped.RowPitch;
	}

	m_context->Unmap(m_stagingTexture.get(), 0);
	{
		std::lock_guard<std::mutex> lock(m_frameMutex);
		FrameView newFrameView{
			.data = std::shared_ptr<const uint8_t[]>(destStart, [destStart](const uint8_t*) {
				std::free(destStart);
			}),
			.width = frameWidth,
			.height = frameHeight,
			.stride = stride,
			.format = PixelFormat::Bgra8
		};
		m_latestFrame = newFrameView;
	};
}
bool WgcPixelCapture::EnsureFramePool(winrt::Windows::Graphics::Capture::Direct3D11CaptureFrame frame) {
	if (!frame)
		return false;
	winrt::Windows::Graphics::SizeInt32 newFrameSize = frame.ContentSize();
	if (newFrameSize.Width != m_currentFramePoolSize.Width || newFrameSize.Height != m_currentFramePoolSize.Height) {
		m_currentFramePoolSize = newFrameSize;
		m_framePool.Recreate(
			m_direct3DDevice,
			winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized,
			2,
			m_currentFramePoolSize
		);
	}
	return true;
}
bool WgcPixelCapture::EnsureStagingTexture(int width, int height) {
	if (m_stagingTexture) {
		D3D11_TEXTURE2D_DESC stagingDesc{};
		m_stagingTexture->GetDesc(&stagingDesc);
		if (stagingDesc.Width == width && stagingDesc.Height == height)
			return true;
	}

	D3D11_TEXTURE2D_DESC desc{};
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE::D3D11_USAGE_STAGING;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_READ;
	desc.BindFlags = 0;
	desc.MiscFlags = 0;


	m_stagingTexture.reset();
	HRESULT result = m_device->CreateTexture2D(&desc, nullptr, m_stagingTexture.put());
	return SUCCEEDED(result);
}
WgcPixelCapture::~WgcPixelCapture() {
	m_status.store(PixelCaptureStatus::Closed);
	{
		std::lock_guard<std::mutex> lock(m_frameMutex);
		if (m_latestFrame.data)
			m_latestFrame = {};
	}
	if (m_framePool) {
		if (m_frameArrivedToken) {
			m_framePool.FrameArrived(m_frameArrivedToken);
			m_frameArrivedToken = {};
		}
		m_framePool.Close();
		m_framePool = nullptr;
	};
	if (m_captureSession) {
		m_captureSession.Close();
		m_captureSession = nullptr;
	}
	m_item = nullptr;
	m_direct3DDevice = nullptr;
}