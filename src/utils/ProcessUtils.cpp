#include <utils/ProcessUtils.h>

std::optional<DWORD> ProcessUtils::MakeProcess(const std::filesystem::path& execPath, std::wstring& commandLine, DWORD timeoutMs) {
	if (!ProcessUtils::IsValidPath(execPath))
		return std::nullopt;

	STARTUPINFOW startupInfo{ sizeof(startupInfo) };
	PROCESS_INFORMATION processInfo{};

	if (!CreateProcessW(execPath.c_str(), commandLine.data(), NULL, NULL, FALSE, 0, NULL, NULL, &startupInfo, &processInfo))
		return std::nullopt;

	DWORD waitResult = WaitForSingleObject(processInfo.hProcess, timeoutMs);
	CloseHandle(processInfo.hProcess);
	CloseHandle(processInfo.hThread);
	if (waitResult == WAIT_TIMEOUT)
		return processInfo.dwProcessId;
	return std::nullopt;
}