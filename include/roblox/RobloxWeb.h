#pragma once

#include <vector>
#include <string>
#include <optional>
#include <http/HttpClient.h>

struct UserData {
	uint64_t id = 0;
	std::string name;
};
struct PrivateServerData {
	std::string accessCode;
	std::string privateServerId;
	std::string serverName;
	uint64_t ownerId = 0;
};
using WebToken = std::optional<std::string>;
class RobloxWeb {
private:
	std::string m_robloSecurity;
	HttpClient m_httpClient;
	std::optional<UserData> m_userData;
public:
	RobloxWeb(const std::string& robloSecurity);
	~RobloxWeb() = default;
	WebToken GetClientAssertion();
	WebToken GetCsrfToken();
	WebToken GetAuthenticationTicket();
	std::optional<UserData> GetUserData();
	std::vector<PrivateServerData> GetPrivateServersFromGame(uint64_t gameId);
};