#include <roblox/RobloxLauncher.h>

struct WindowSearchData {
	DWORD targetProcessId = 0;
	HWND foundHwnd = nullptr;
};


RobloxLauncher::RobloxLauncher(std::filesystem::path gamePath) : m_gamePath(gamePath) {}


std::optional<DWORD> RobloxLauncher::MakeProcess(std::wstring& commandLine) {
	if (!IsValidPath())
		return std::nullopt;

	STARTUPINFOW startupInfo{ sizeof(startupInfo) };
	PROCESS_INFORMATION processInfo{};

	if (!CreateProcessW(m_gamePath.c_str(), commandLine.data(), NULL, NULL, FALSE, 0, NULL, NULL, &startupInfo, &processInfo))
		return std::nullopt;

	DWORD waitResult = WaitForSingleObject(processInfo.hProcess, 10000);
	CloseHandle(processInfo.hProcess);
	CloseHandle(processInfo.hThread);
	if (waitResult == WAIT_TIMEOUT)
		return processInfo.dwProcessId;
	return std::nullopt;
}

std::optional<DWORD> RobloxLauncher::JoinPublicGame(uint64_t gameId, const std::string& authTicket) {
	std::wstring commandLine = std::format(
		L"\"{}\" \"roblox-player:1+launchmode:play+gameinfo:{}+placelauncherurl:https%3A%2F%2Fwww.roblox.com%2FGame%2FPlaceLauncher.ashx%3Frequest%3DRequestGame%26placeId%3D{}\"",
		m_gamePath.wstring(),
		std::wstring(authTicket.begin(), authTicket.end()),
		std::to_wstring(gameId)
	);

	return MakeProcess(commandLine);
}

std::optional<DWORD> RobloxLauncher::JoinPrivateGame(uint64_t gameId, const std::string& accessCode, const std::string& authTicket) {
	std::wstring commandLine = std::format(
		L"\"{}\" \"roblox-player:1+launchmode:play+gameinfo:{}+placelauncherurl:https%3A%2F%2Fwww.roblox.com%2FGame%2FPlaceLauncher.ashx%3Frequest%3DRequestPrivateGame%26accessCode%3D{}%26placeId%3D{}\"",
		m_gamePath.wstring(),
		std::wstring(authTicket.begin(), authTicket.end()),
		std::wstring(accessCode.begin(), accessCode.end()),
		std::to_wstring(gameId)
	);

	return MakeProcess(commandLine);
}