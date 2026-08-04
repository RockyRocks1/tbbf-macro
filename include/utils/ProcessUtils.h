#pragma once
#include <windows.h>
#include <filesystem>
#include <optional>
#include <string>


namespace ProcessUtils {
    inline bool IsValidPath(const std::filesystem::path& path) {
        return std::filesystem::exists(path);
    };
    std::optional<DWORD> MakeProcess(const std::filesystem::path& execPath, std::wstring& commandLine, DWORD timeoutMs);
}