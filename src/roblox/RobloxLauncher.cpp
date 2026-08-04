#include <roblox/RobloxLauncher.h>

RobloxLauncher::RobloxLauncher(std::filesystem::path gamePath) : m_gamePath(std::move(gamePath)) {}

std::optional<DWORD> RobloxLauncher::JoinPublicGame(uint64_t gameId, const std::string& authTicket) {
	std::wstring commandLine = std::format(
		L"\"{}\" \"roblox-player:1+launchmode:play+gameinfo:{}+placelauncherurl:https%3A%2F%2Fwww.roblox.com%2FGame%2FPlaceLauncher.ashx%3Frequest%3DRequestGame%26placeId%3D{}\"",
		m_gamePath.wstring(),
		std::wstring(authTicket.begin(), authTicket.end()),
		std::to_wstring(gameId)
	);

	return ProcessUtils::MakeProcess(m_gamePath, commandLine, 10000);
}

std::optional<DWORD> RobloxLauncher::JoinPrivateGame(uint64_t gameId, const std::string& accessCode, const std::string& authTicket) {
	std::wstring commandLine = std::format(
		L"\"{}\" \"roblox-player:1+launchmode:play+gameinfo:{}+placelauncherurl:https%3A%2F%2Fwww.roblox.com%2FGame%2FPlaceLauncher.ashx%3Frequest%3DRequestPrivateGame%26accessCode%3D{}%26placeId%3D{}\"",
		m_gamePath.wstring(),
		std::wstring(authTicket.begin(), authTicket.end()),
		std::wstring(accessCode.begin(), accessCode.end()),
		std::to_wstring(gameId)
	);

	return ProcessUtils::MakeProcess(m_gamePath, commandLine, 10000);
}