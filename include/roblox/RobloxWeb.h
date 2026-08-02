#include <string>
#include <map>
#include <unordered_set>
#include <optional>
#include <http/HttpClient.h>

struct UserInformation {
	uint64_t id = 0;
	std::string name;
};
using WebToken = std::optional<std::string>;
class RobloxWeb {
private:
	std::string m_robloSecurity;
	HttpClient m_httpClient;
	std::optional<UserInformation> m_userInfo;
public:
	RobloxWeb(const std::string& robloSecurity);
	~RobloxWeb() = default;
	WebToken GetClientAssertion();
	WebToken GetCsrfToken();
	WebToken GetAuthenticationTicket();
	std::optional<UserInformation> GetUserInformation();
	void GetPrivateServers();
	bool OwnsBadge(size_t badgeID);
};