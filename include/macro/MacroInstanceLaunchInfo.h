#pragma once
#include <string>
#include <filesystem>

struct MacroInstanceLaunchInfo {
    std::string robloSecurity = "";
    uint64_t gameId = 0;
    std::string privateServerAccessCode = "";
    std::filesystem::path gamePath = "";
};