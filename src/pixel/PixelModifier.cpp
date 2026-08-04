#include <pixel/PixelModifier.h>

template<typename Func>
bool PixelModifier::Map1to1(const FrameView& sourceView, FrameBuffer& destBuffer, Func iteratorFunction) {
	if (sourceView.width != destBuffer.width || sourceView.height != destBuffer.height)
		return false;
	if (!sourceView.data || !destBuffer.data.data())
		return false;

	const uint32_t sourceBytesPerPixel = sourceView.GetBytesPerPixel();
	const uint32_t destBytesPerPixel = destBuffer.GetBytesPerPixel();
	
	for (int y = 0; y < sourceView.height; y++) {
		const uint8_t* pSourceRow = static_cast<const uint8_t*>(sourceView.data + sourceView.stride * y);
		uint8_t* pDestRow = destBuffer.data.data() + destBuffer.stride * y;

		const uint8_t* pSourcePixel = pSourceRow;
		uint8_t* pDestPixel = pDestRow;
		for (int x = 0; x < sourceView.width; x++) {
			iteratorFunction(pSourcePixel, pDestPixel);
			pSourcePixel += sourceBytesPerPixel;
			pDestPixel += destBytesPerPixel;
		}
	}
	return true;
}

FrameView PixelModifier::Crop(const FrameView& sourceView, uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
	if (x + width > sourceView.width || y + height > sourceView.height)
		return {};
	FrameView view{};
	view.data = sourceView.data + y * sourceView.stride + x * sourceView.GetBytesPerPixel();
	view.width = width;
	view.height = height;
	view.format = sourceView.format;
	view.stride = sourceView.stride;

	return view;
}

bool PixelModifier::Grayscale(const FrameView& sourceView, FrameBuffer& destBuffer) {
	if (sourceView.format != PixelFormat::Bgra8)
		return false;
	const uint32_t newStride = FrameBuffer::GetOptimalStride(sourceView.width);
	const size_t vectorSize = static_cast<size_t>(newStride) * sourceView.height;
	
	destBuffer.data.resize(vectorSize);
	destBuffer.width = sourceView.width;
	destBuffer.height = sourceView.height;
	destBuffer.format = PixelFormat::Gray8;
	destBuffer.stride = newStride;
	
	auto grayscale = [](const uint8_t* src, uint8_t* dest) {
		*dest = (src[static_cast<uint8_t>(BgraChannel::R)] * 54 +
			src[static_cast<uint8_t>(BgraChannel::G)] * 183 + 
			src[static_cast<uint8_t>(BgraChannel::B)] * 18) >> 8;
	};

	return Map1to1(sourceView, destBuffer, grayscale);
}

bool PixelModifier::Invert(const FrameView& sourceView, FrameBuffer& destBuffer) {
	const size_t vectorSize = sourceView.GetBufferSize();

	destBuffer.data.resize(vectorSize);
	destBuffer.width = sourceView.width;
	destBuffer.height = sourceView.height;
	destBuffer.format = sourceView.format;
	destBuffer.stride = sourceView.stride;

	
	switch(sourceView.format) {
	case PixelFormat::Bgra8: {
		auto invertBgra = [](const uint8_t* src, uint8_t* dest) {
			uint32_t srcPixel = *reinterpret_cast<const uint32_t*>(src);
			*reinterpret_cast<uint32_t*>(dest) = srcPixel ^ 0x00FFFFFF;
			};
		return Map1to1(sourceView, destBuffer, invertBgra);
	}
	case PixelFormat::Gray8: {
		auto invertGray = [](const uint8_t* src, uint8_t* dest) {
			*dest = ~src[0];
			};
		return Map1to1(sourceView, destBuffer, invertGray);
	}
	default:
		return false;
	}
}
bool PixelModifier::Threshold(const FrameView& sourceView, FrameBuffer& destBuffer, uint8_t thresholdVal, ThresholdType thresholdType) {
	if (sourceView.format != PixelFormat::Gray8)
		return false;

	const size_t vectorSize = sourceView.GetBufferSize();

	destBuffer.data.resize(vectorSize);
	destBuffer.width = sourceView.width;
	destBuffer.height = sourceView.height;
	destBuffer.format = sourceView.format;
	destBuffer.stride = sourceView.stride;


	switch (thresholdType) {
	case ThresholdType::BINARY: {
		auto binary = [thresholdVal](const uint8_t* src, uint8_t* dest) {
			*dest = (src[0] < thresholdVal) ? 0 : 255;
			};
		return Map1to1(sourceView, destBuffer, binary);
	}
	case ThresholdType::BINARY_INV: {
		auto binaryInv = [thresholdVal](const uint8_t* src, uint8_t* dest) {
			*dest = (src[0] < thresholdVal) ? 255 : 0;
			};
		return Map1to1(sourceView, destBuffer, binaryInv);
	}
	case ThresholdType::TRUNC: {
		auto trunca = [thresholdVal](const uint8_t* src, uint8_t* dest) {
			*dest = (src[0] > thresholdVal) ? thresholdVal : src[0];
			};
		return Map1to1(sourceView, destBuffer, trunca);
	}
	case ThresholdType::TO_ZERO: {
		auto toZero = [thresholdVal](const uint8_t* src, uint8_t* dest) {
			*dest = (src[0] < thresholdVal) ? 0 : src[0];
			};
		return Map1to1(sourceView, destBuffer, toZero);
	}
	case ThresholdType::TO_ZERO_INV: {
		auto toZeroInv = [thresholdVal](const uint8_t* src, uint8_t* dest) {
			*dest = (src[0] > thresholdVal) ? 0 : src[0];
			};
		return Map1to1(sourceView, destBuffer, toZeroInv);
	}
	default:
		return false;
	};
}