#include <Windows.h>
#include <string>
#include <filesystem>
#include <optional>

class RobloxLauncher {
private:
	std::filesystem::path m_gamePath;

	std::optional<DWORD> MakeProcess(std::wstring& commandLine);
public:
	RobloxLauncher(std::filesystem::path gamePath);

	std::optional<DWORD> JoinPublicGame(uint64_t gameId, const std::string& authTicket);
	std::optional<DWORD> JoinPrivateGame(uint64_t gameId, const std::string& accessCode, const std::string& authTicket);
	
	inline bool IsValidPath() const {
		return std::filesystem::exists(m_gamePath);
	};
};