#include "http/HttpClient.h"

HttpClient::HttpClient(): m_curl(curl_easy_init(), curl_easy_cleanup), m_headerList(nullptr, curl_slist_free_all) {}

void HttpClient::getCookieString(std::string& outCookieString) const {
	outCookieString.clear();
	for (const auto& [cookieName, cookieValue] : m_cookies)
		outCookieString += std::format("{}={}; ", cookieName, cookieValue);

	if (outCookieString.length() >= 2) {
		outCookieString.pop_back(); // removes space
		outCookieString.pop_back(); // removes ;
	}
}

bool HttpClient::SetupRequest(const std::string& url, const StringMap& headers, Response& response) {
	if (!m_curl)
		return false;

	curl_easy_reset(m_curl.get());
	m_headerList.reset();

	curl_easy_setopt(m_curl.get(), CURLOPT_DEFAULT_PROTOCOL, "https");
	curl_easy_setopt(m_curl.get(), CURLOPT_URL, url.c_str());

	getCookieString(m_cookieString);
	curl_easy_setopt(m_curl.get(), CURLOPT_COOKIE, m_cookieString.c_str());

	curl_slist* rawHeaderList = nullptr;
	for (const auto& [headerName, headerValue] : headers) {
		std::string headerEntry = std::format("{}: {}", headerName, headerValue);
		curl_slist* updatedList = curl_slist_append(rawHeaderList, headerEntry.c_str());
		if (updatedList)
			rawHeaderList = updatedList;
	}

	m_headerList = CurlSlistUniquePtr(rawHeaderList, curl_slist_free_all);
	curl_easy_setopt(m_curl.get(), CURLOPT_HTTPHEADER, m_headerList.get());

	curl_easy_setopt(m_curl.get(), CURLOPT_WRITEFUNCTION, HttpClient::SharedCallback);
	curl_easy_setopt(m_curl.get(), CURLOPT_WRITEDATA, &response.body);

	curl_easy_setopt(m_curl.get(), CURLOPT_HEADERFUNCTION, HttpClient::SharedCallback);
	curl_easy_setopt(m_curl.get(), CURLOPT_HEADERDATA, &response.headers);
	
	return true;
}
Response HttpClient::get(const std::string& url, const StringMap& headers) {
	Response response;
	if (!SetupRequest(url, headers, response)) {
		response.code = CURLE_FAILED_INIT;
		return response;
	}

	response.code = curl_easy_perform(m_curl.get());
	return response;
}
Response HttpClient::post(const std::string& url, const std::string& body, const StringMap& headers) {
	Response response;
	if (!SetupRequest(url, headers, response)) {
		response.code = CURLE_FAILED_INIT;
		return response;
	}
	curl_easy_setopt(m_curl.get(), CURLOPT_POST, 1L);
	curl_easy_setopt(m_curl.get(), CURLOPT_POSTFIELDS, body.c_str());
	curl_easy_setopt(m_curl.get(), CURLOPT_POSTFIELDSIZE, static_cast<long>(body.size()));
	
	response.code = curl_easy_perform(m_curl.get());
	return response;
}
size_t HttpClient::SharedCallback(char* ptr, size_t size, size_t nmemb, void* userdata) {
	size_t totalSize = size * nmemb;
	std::string* response_body = static_cast<std::string*>(userdata);
	response_body->append(ptr, totalSize);
	return totalSize;
}
