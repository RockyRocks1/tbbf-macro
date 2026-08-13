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
		const uint8_t* pSourceRow = static_cast<const uint8_t*>(sourceView.data.get() + sourceView.stride * y);
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

FrameView PixelModifier::Crop(const FrameView& sourceView, const Rect& cropRegion) {
	if (cropRegion.width <= 0 || cropRegion.height <= 0 || cropRegion.x + cropRegion.width > sourceView.width || cropRegion.y + cropRegion.height > sourceView.height)
		return {};
	size_t byteOffset = (static_cast<size_t>(cropRegion.y) * sourceView.stride) + (static_cast<size_t>(cropRegion.x) * sourceView.GetBytesPerPixel());
	std::shared_ptr<const uint8_t[]> croppedData(sourceView.data, sourceView.data.get() + byteOffset);

	return FrameView{
		.data = croppedData,
		.width = cropRegion.width,
		.height = cropRegion.height,
		.stride = sourceView.stride,
		.format = sourceView.format
	};
}

bool PixelModifier::Grayscale(const FrameView& sourceView, FrameBuffer& destBuffer) {
	if (sourceView.format != PixelFormat::Bgra8)
		return false;
	const size_t newStride = FrameBuffer::GetOptimalStride(sourceView.width);
	const size_t vectorSize = newStride * sourceView.height;
	
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

bool PixelModifier::Censor(const FrameView& sourceView, FrameBuffer& destBuffer, const Rect& censorRegion) {
	if (censorRegion.width <= 0 || censorRegion.height <= 0 || censorRegion.x + censorRegion.width > sourceView.width || censorRegion.y + censorRegion.height > sourceView.height)
		return false;
	if (sourceView.format != PixelFormat::Gray8)
		return false;
	
	const size_t vectorSize = sourceView.GetBufferSize();

	destBuffer.data.resize(vectorSize);
	destBuffer.width = sourceView.width;
	destBuffer.height = sourceView.height;
	destBuffer.format = sourceView.format;
	destBuffer.stride = sourceView.stride;


	const int censorRight = censorRegion.x + censorRegion.width;
	const int censorBottom = censorRegion.y + censorRegion.height;

	for (int y = 0; y < sourceView.height; y++) {
		const uint8_t* pSourceRow = static_cast<const uint8_t*>(sourceView.data.get() + sourceView.stride * y);
		uint8_t* pDestRow = destBuffer.data.data() + destBuffer.stride * y;

		const uint8_t* pSourcePixel = pSourceRow;
		uint8_t* pDestPixel = pDestRow;
		for (int x = 0; x < sourceView.width; x++) {
			bool shouldCensor = x >= censorRegion.x && x < censorRight && y >= censorRegion.y && y < censorBottom;
			*pDestPixel = (shouldCensor) ? 0 : *pSourcePixel;
			pSourcePixel++;
			pDestPixel++;
		}
	}
	return true;
}

bool PixelModifier::Upscale(const FrameView& sourceView, FrameBuffer& destBuffer, int scaleFactor) {
	if (scaleFactor <= 0)
		return false;
	if (sourceView.format != PixelFormat::Gray8)
		return false;

	destBuffer.width = sourceView.width * scaleFactor;
	destBuffer.height = sourceView.height * scaleFactor;
	destBuffer.format = sourceView.format;

	const size_t newStride = FrameBuffer::GetOptimalStride(destBuffer.width);
	const size_t vectorSize = newStride * destBuffer.height;

	destBuffer.data.resize(vectorSize);
	destBuffer.stride = newStride;
	for (int y = 0; y < destBuffer.height; y++) {
		const uint8_t* pSourceRow = static_cast<const uint8_t*>(sourceView.data.get() + sourceView.stride * (y / scaleFactor));
		uint8_t* pDestRow = destBuffer.data.data() + destBuffer.stride * y;

		uint8_t* pDestPixel = pDestRow;
		for (int x = 0; x < destBuffer.width; x++) {
			const uint8_t pSourcePixel = pSourceRow[x / scaleFactor];
			*pDestPixel = pSourcePixel;
			pDestPixel++;
		}
	}
	return true;
}

bool PixelModifier::Normalize(const FrameView& sourceView, FrameBuffer& destBuffer, const Rect& ignoreRegion) {
	if (sourceView.format != PixelFormat::Gray8)
		return false;

	const size_t vectorSize = sourceView.GetBufferSize();

	destBuffer.data.resize(vectorSize);
	destBuffer.width = sourceView.width;
	destBuffer.height = sourceView.height;
	destBuffer.format = sourceView.format;
	destBuffer.stride = sourceView.stride;

	const int ignoreRight = ignoreRegion.x + ignoreRegion.width;
	const int ignoreBottom = ignoreRegion.y + ignoreRegion.height;

	uint8_t minVal = 255;
	uint8_t maxVal = 0;
	for (int y = 0; y < sourceView.height; y++) {
		const uint8_t* pRow = sourceView.data.get() + (sourceView.stride * y);
		const bool isRowInsideIgnoreZone = (y >= ignoreRegion.y && y < ignoreBottom);

		for (int x = 0; x < sourceView.width; x++) {
			if (isRowInsideIgnoreZone && x >= ignoreRegion.x && x < ignoreRight)
				continue;
			
			if (pRow[x] < minVal)
				minVal = pRow[x];
			if (pRow[x] > maxVal)
				maxVal = pRow[x];
		}
	}

	if (maxVal == minVal)
		return false;

	const float scale = 255.0f / (maxVal - minVal);
	uint8_t lookUpTable[256] = {};
	
	for (int i = 0; i < 256; i++) {
		float normalized = (i - minVal) * scale;
		lookUpTable[i] = static_cast<uint8_t>(std::clamp(normalized, 0.0f, 255.0f));
	}

	auto normalize = [lookUpTable](const uint8_t* src, uint8_t* dest) {
		*dest = lookUpTable[*src];
		};
	return Map1to1(sourceView, destBuffer, normalize);
}
