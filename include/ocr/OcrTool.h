#pragma once
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.System.Threading.h>
#include <Windows.h>
#include <winrt/Windows.Media.Ocr.h>
#include <winrt/Windows.Graphics.Imaging.h>
#include <MemoryBuffer.h>
#include <pixel/FrameView.h>
#include <string>

// Warning: this tool is relatively slow, use sparingly!
class OcrTool {
public:
	inline static const winrt::Windows::Media::Ocr::OcrEngine engine = winrt::Windows::Media::Ocr::OcrEngine::TryCreateFromUserProfileLanguages();
	static std::string RecognizeText(const FrameView& frameView);
	static std::string FindClosestMatch(const std::string& srcStr, const std::vector<std::string>& targetStrs);
private:
	static int GetLevenshteinDistance(std::string_view srcStr, std::string_view destStr);
};