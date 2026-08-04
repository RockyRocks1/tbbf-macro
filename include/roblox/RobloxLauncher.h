#pragma once

#include <utils/ProcessUtils.h>

class RobloxLauncher {
private:
	std::filesystem::path m_gamePath;

public:
	RobloxLauncher(std::filesystem::path gamePath);

	std::optional<DWORD> JoinPublicGame(uint64_t gameId, const std::string& authTicket);
	std::optional<DWORD> JoinPrivateGame(uint64_t gameId, const std::string& accessCode, const std::string& authTicket);
};