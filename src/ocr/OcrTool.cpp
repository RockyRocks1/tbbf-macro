#include <ocr/OcrTool.h>

winrt::Windows::Graphics::Imaging::SoftwareBitmap CreateSoftwareBitmapFromFrameView(const FrameView& frameView) {
	using namespace winrt::Windows::Graphics::Imaging;

	BitmapPixelFormat winrtFormat = BitmapPixelFormat::Rgba8;
	BitmapAlphaMode alphaMode = BitmapAlphaMode::Premultiplied;
	
	switch (frameView.format) {
	case PixelFormat::Gray8:
		alphaMode = BitmapAlphaMode::Ignore;
		winrtFormat = BitmapPixelFormat::Gray8;
		break;
	case PixelFormat::Bgra8:
	default:
		break;
	}
	
	SoftwareBitmap bitmap(winrtFormat, frameView.width, frameView.height, alphaMode);

	BitmapBuffer bitmapBuffer = bitmap.LockBuffer(BitmapBufferAccessMode::Write);
	winrt::Windows::Foundation::IMemoryBufferReference reference = bitmapBuffer.CreateReference();

	uint8_t* dstData = nullptr;
	uint32_t capacity = 0;

	auto byteAccess = reference.as<Windows::Foundation::IMemoryBufferByteAccess>();
	winrt::check_hresult(byteAccess->GetBuffer(&dstData, &capacity));

	BitmapPlaneDescription planeDesc = bitmapBuffer.GetPlaneDescription(0);
	const uint32_t dstStride = planeDesc.Stride;
	const size_t bytesPerPixelRow = static_cast<size_t>(frameView.GetBytesPerPixel() * frameView.width);
	for (int y = 0; y < frameView.height; ++y) {
		const uint8_t* srcRow = frameView.data.get() + (y * frameView.stride);
		uint8_t* dstRow = dstData + (y * dstStride);

		std::memcpy(dstRow, srcRow, bytesPerPixelRow);
	}
	return bitmap;
}

std::string OcrTool::RecognizeText(const FrameView& frameView) {
	using namespace winrt::Windows::Graphics::Imaging;
	if (!engine)
		return "";

	SoftwareBitmap bitmap = CreateSoftwareBitmapFromFrameView(frameView);
	if (!bitmap)
		return "";

	winrt::Windows::Media::Ocr::OcrResult result = engine.RecognizeAsync(bitmap).get();

	return winrt::to_string(result.Text());
};