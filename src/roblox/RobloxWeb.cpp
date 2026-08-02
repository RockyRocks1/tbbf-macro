#include <roblox/RobloxWeb.h>

RobloxWeb::RobloxWeb(const std::string& robloSecurity) : m_robloSecurity(robloSecurity) {
	m_httpClient.setCookie(".ROBLOSECURITY", robloSecurity);
}
WebToken RobloxWeb::GetCsrfToken() {
	StringMap headers = {
		{"Referer", "https://www.roblox.com"}
	};
	HttpResponse response = m_httpClient.post("https://auth.roblox.com/v1/authentication-ticket/", "", headers);
	if (!response.headers.contains("x-csrf-token"))
		return std::nullopt;
	return response.headers.at("x-csrf-token");
}
WebToken RobloxWeb::GetClientAssertion() {
	StringMap headers = {
		{"Referer", "https://www.roblox.com"}
	};
	HttpResponse response = m_httpClient.get("https://auth.roblox.com/v1/client-assertion/", headers);
	if (response.statusCode != 200)
		return std::nullopt;

	try {
		nlohmann::json responseJson = response.json();
		if (!responseJson.contains("clientAssertion"))
			return std::nullopt;
		return responseJson["clientAssertion"].get<std::string>();
	}
	catch (const nlohmann::json::exception&) {
		return std::nullopt;
	}
}
std::optional<std::string> RobloxWeb::GetAuthenticationTicket() {
	WebToken clientAssertion = GetClientAssertion();
	if (!clientAssertion.has_value())
		return std::nullopt;
	WebToken csrfToken = GetCsrfToken();
	if (!csrfToken.has_value())
		return std::nullopt;

	StringMap headers = {
		{"Referer", "https://www.roblox.com"},
		{"x-csrf-token", csrfToken.value()}
	};

	nlohmann::json requestBody = {
		{"clientAssertion", clientAssertion.value()}
	};
	HttpResponse response = m_httpClient.post("https://auth.roblox.com/v1/authentication-ticket/", requestBody.dump(), headers);
	if (!response.headers.contains("rbx-authentication-ticket"))
		return std::nullopt;
	return response.headers.at("rbx-authentication-ticket");
}

std::optional<UserInformation> RobloxWeb::GetUserInformation() {
	if (m_userInfo.has_value())
		return m_userInfo;
	StringMap headers = {
		{"Referer", "https://www.roblox.com"}
	};
	HttpResponse response = m_httpClient.get("https://users.roblox.com/v1/users/authenticated/", headers);
	if (response.statusCode != 200)
		return std::nullopt;
	try {
		nlohmann::json responseJson = response.json();
		if (!responseJson.contains("id") || !responseJson.contains("name"))
			return std::nullopt;
		UserInformation userInfo;
		userInfo.id = responseJson["id"].get<uint64_t>();
		userInfo.name = responseJson["name"].get<std::string>();
		m_userInfo = userInfo;
		return m_userInfo;
	}
	catch (const nlohmann::json::exception&) {
		return std::nullopt;
	}
}