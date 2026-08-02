#include <string>
#include <curl/curl.h>
struct RobloxTokens {
	std::string csrf;
	std::string clientAssertion;
};
class RobloxWeb {
private:
	const std::string m_robloSecurity;
	RobloxTokens m_tokens{};
public:
	RobloxWeb(std::string robloSecurity) : m_robloSecurity(robloSecurity) {};
	~RobloxWeb() = default;
	
};