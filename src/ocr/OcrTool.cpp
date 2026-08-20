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

	try {
		winrt::Windows::Media::Ocr::OcrResult result{ nullptr };

		winrt::Windows::System::Threading::ThreadPool::RunAsync([&](auto&&) {
			result = engine.RecognizeAsync(bitmap).get();
			}).get(); 
		if (result) 
			return winrt::to_string(result.Text());
		
	}
	catch (winrt::hresult_error const&) {}

	return "";
};

int OcrTool::GetLevenshteinDistance(std::string_view srcStr, std::string_view destStr) {
	const size_t srcStrLength = srcStr.length();
	const size_t destStrLength = destStr.length();

	if (srcStrLength == 0)
		return static_cast<int>(destStrLength);
	if (destStrLength == 0)
		return static_cast<int>(srcStrLength);


	std::vector<int> dp(destStrLength + 1, 0);

	for (size_t j = 0; j <= destStrLength; j++)
		dp[j] = static_cast<int>(j);


	for (size_t i = 1; i <= srcStrLength; i++) {
		int prevDiagonal = dp[0];
		dp[0] = static_cast<int>(i);

		for (size_t j = 1; j <= destStrLength; j++) {
			const int nextDiagonal = dp[j];
			const int cost = (srcStr[i - 1] == destStr[j - 1]) ? 0 : 1;

			const int deletion = dp[j] + 1;
			const int insertion = dp[j - 1] + 1;
			const int subsitution = prevDiagonal + cost;

			dp[j] = (std::min)({ deletion, insertion, subsitution });
			prevDiagonal = nextDiagonal;
		}
	}

	return dp[destStrLength];
}

std::string OcrTool::FindClosestMatch(const std::string& srcStr, const std::vector<std::string>& targetStrs) {
	int closestDistance = MAXINT;
	std::string closestMatch;
	for (const std::string& targetStr : targetStrs) {
		int distance = GetLevenshteinDistance(srcStr, targetStr);
		if (distance >= closestDistance)
			continue;
		closestDistance = distance;
		closestMatch = targetStr;
	}
	return closestMatch;
}