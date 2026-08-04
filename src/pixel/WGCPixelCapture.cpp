#include <pixel/WgcPixelCapture.h>



bool WgcPixelCapture::Initialize(HWND targetHwnd) {
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
	m_captureSession.StartCapture();
	return true;
}
void WgcPixelCapture::OnFrameArrived(const winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool& sender, const winrt::Windows::Foundation::IInspectable& args) {
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
bool WgcPixelCapture::EnsureStagingTexture(uint32_t width, uint32_t height)
{
	if (m_stagingTexture)
	{
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
bool WgcPixelCapture::CaptureRegion(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
	if (width <= 0 || height <= 0 || x < 0 || y < 0)
		return false;

	Rect clientBounds = WindowUtils::GetClientRectRelativeToWindow(m_targetHwnd);
	const uint32_t clientX = static_cast<uint32_t>(clientBounds.x) + x;
	const uint32_t clientY = static_cast<uint32_t>(clientBounds.y) + y;

	if (clientX + width > static_cast<uint32_t>(clientBounds.width) ||
		clientY + height > static_cast<uint32_t>(clientBounds.height)) // there could be an one-by-one runtime error here...
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
	const uint32_t frameWidth = sourceDesc.Width;
	const uint32_t frameHeight = sourceDesc.Height;

	
	if (!EnsureStagingTexture(width, height))
		return false;

	D3D11_BOX box{};
	box.left = clientX;
	box.top = clientY;
	box.front = 0;
	box.right = clientX + width;
	box.bottom = clientY + height;
	box.back = 1;
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
	size_t requiredBytes = static_cast<size_t>(width) * height * 4;
	if (requiredBytes > m_bufferCapacity) {
		void* newBuffer = std::realloc(m_pBuffer, requiredBytes);
		if (!newBuffer) {
			m_context->Unmap(m_stagingTexture.get(), 0);
			return false;
		}
		m_pBuffer = newBuffer;
		m_bufferCapacity = requiredBytes;
	}

	m_width = width;
	m_height = height;
	uint8_t* sourceBytes = static_cast<uint8_t*>(mapped.pData);
	uint8_t* destBytes = static_cast<uint8_t*>(m_pBuffer);

	for (int row = 0; row < height; ++row)
	{
		std::memcpy(destBytes, sourceBytes, static_cast<size_t>(width) * 4);
		destBytes += width * 4;
		sourceBytes += mapped.RowPitch;
	}

	m_context->Unmap(m_stagingTexture.get(), 0);
	return true;
}

WgcPixelCapture::~WgcPixelCapture() {
	if (m_pBuffer)
	{
		std::free(m_pBuffer);
		m_pBuffer = nullptr;
	}
	// TODO: add the rest of the cleanup....
}