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
	m_status.store(PixelCaptureStatus::Closed);
	if (m_pBuffer) {
		std::free(m_pBuffer);
		m_pBuffer = nullptr;
	}
	if (m_framePool) {
		if (m_frameArrivedToken) {
			m_framePool.FrameArrived(m_frameArrivedToken);
			m_frameArrivedToken = {};
		}
		m_framePool.Close();
		m_framePool = nullptr;
	};
}
void WgcPixelCapture::OnFrameArrived(const winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool& sender, const winrt::Windows::Foundation::IInspectable& args) {
	if (m_status.load() != PixelCaptureStatus::Running)
		return;
	winrt::Windows::Graphics::Capture::Direct3D11CaptureFrame frame = sender.TryGetNextFrame();
	if (!frame)
		return;
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
	auto access = frame.Surface().as<Windows::Graphics::DirectX::Direct3D11::IDirect3DDxgiInterfaceAccess>();
	wil::com_ptr<ID3D11Texture2D> newSourceTexture;
	HRESULT result = access->GetInterface(IID_PPV_ARGS(newSourceTexture.put()));
	if (FAILED(result))
		return;
	{
		std::lock_guard<std::mutex> lock(m_frameMutex);
		m_currentSourceTexture = newSourceTexture;
	}
}
bool WgcPixelCapture::EnsureStagingTexture(int width, int height)
{
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

bool WgcPixelCapture::CaptureRegion(const Rect& region) {
	if (m_status.load() != PixelCaptureStatus::Running)
		return false;

	if (region.width <= 0 || region.height <= 0 || region.x < 0 || region.y < 0)
		return false;

	if (!m_context)
		return false;

	wil::com_ptr<ID3D11Texture2D> localTexture;
	{
		std::lock_guard<std::mutex> lock(m_frameMutex);
		localTexture = m_currentSourceTexture;
	}
	if (!localTexture)
		return false;

	D3D11_TEXTURE2D_DESC sourceDesc{};
	localTexture->GetDesc(&sourceDesc);
	
	D3D11_BOX box{};
	box.left = static_cast<uint32_t>(region.x);
	box.top = static_cast<uint32_t>(region.y);
	box.front = 0;
	box.right = box.left + static_cast<uint32_t>(region.width);
	box.bottom = box.top + static_cast<uint32_t>(region.height);
	box.back = 1;

	if (box.right > sourceDesc.Width || box.bottom > sourceDesc.Height)
		return false;

	if (!EnsureStagingTexture(region.width, region.height))
		return false;

	m_context->CopySubresourceRegion(
		m_stagingTexture.get(), 0,
		0, 0, 0,
		localTexture.get(), 0,
		&box
	);

	D3D11_MAPPED_SUBRESOURCE mapped{};
	HRESULT result = m_context->Map(m_stagingTexture.get(), 0, D3D11_MAP::D3D11_MAP_READ, 0, &mapped);
	if (FAILED(result))
		return false;
	const size_t rowPitchBytes = static_cast<size_t>(region.width) * 4;
	const size_t requiredBytes = rowPitchBytes * region.height;

	if (requiredBytes > m_bufferCapacity) {
		void* newBuffer = std::realloc(m_pBuffer, requiredBytes);
		if (!newBuffer) {
			m_context->Unmap(m_stagingTexture.get(), 0);
			return false;
		}
		m_pBuffer = newBuffer;
		m_bufferCapacity = requiredBytes;
	}
	
	m_width = region.width;
	m_height = region.height;

	const uint8_t* sourceBytes = static_cast<const uint8_t*>(mapped.pData);
	uint8_t* destBytes = static_cast<uint8_t*>(m_pBuffer);

	for (int row = 0; row < region.height; row++) {
		std::memcpy(destBytes, sourceBytes, rowPitchBytes);
		destBytes += rowPitchBytes;
		sourceBytes += mapped.RowPitch;
	}

	m_context->Unmap(m_stagingTexture.get(), 0);
	return true;
}

bool WgcPixelCapture::CaptureClientRegion(const Rect& clientRegion) {
	if (clientRegion.width <= 0 || clientRegion.height <= 0 || clientRegion.x < 0 || clientRegion.y < 0)
		return false;

	std::optional<POINT> clientOffset = WindowUtils::GetClientOffsetFromWgc(m_targetHwnd);

	if (!clientOffset)
		return false;
	Rect windowRegion{
		clientRegion.x + clientOffset->x,
		clientRegion.y + clientOffset->y,
		clientRegion.width,
		clientRegion.height
	};

	return CaptureRegion(windowRegion);
}
WgcPixelCapture::~WgcPixelCapture() {
	Close();
}