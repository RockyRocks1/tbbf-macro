#include "http/HttpClient.h"

HttpClient::HttpClient(): m_curl(curl_easy_init(), curl_easy_cleanup) {}

std::string HttpClient::getCookieString() const {
	std::string cookieString;
	for (const auto& [cookieName, cookieValue] : m_cookies)
		cookieString += std::format("{}={}; ", cookieName, cookieValue);
	if (cookieString.length() >= 2) {
		cookieString.pop_back(); // removes space
		cookieString.pop_back(); // removes ;
	}
	return cookieString;
}

Response HttpClient::get(const std::string& url, const StringMap& headers) {
	Response response;
	if (!m_curl) {
		response.code = CURLE_FAILED_INIT;
		return response;
	}

	curl_easy_reset(m_curl.get());
	curl_easy_setopt(m_curl.get(), CURLOPT_DEFAULT_PROTOCOL, "https");
	curl_easy_setopt(m_curl.get(), CURLOPT_URL, url.c_str());

	std::string cookieString = getCookieString();
	curl_easy_setopt(m_curl.get(), CURLOPT_COOKIE, cookieString.c_str());
	
	curl_slist* rawHeaderList = nullptr;
	for (const auto& [headerName, headerValue] : headers) {
		std::string headerEntry = std::format("{}: {}", headerName, headerValue);
		curl_slist* updatedList = curl_slist_append(rawHeaderList, headerEntry.c_str());
		if (updatedList)
			rawHeaderList = updatedList;
	}

	CurlSlistUniquePtr headerList(rawHeaderList, curl_slist_free_all);
	curl_easy_setopt(m_curl.get(), CURLOPT_HTTPHEADER, headerList.get());
	
	curl_easy_setopt(m_curl.get(), CURLOPT_WRITEFUNCTION, HttpClient::SharedCallback);
	curl_easy_setopt(m_curl.get(), CURLOPT_WRITEDATA, &response.body);

	curl_easy_setopt(m_curl.get(), CURLOPT_HEADERFUNCTION, HttpClient::SharedCallback);
	curl_easy_setopt(m_curl.get(), CURLOPT_HEADERDATA, &response.headers);

	response.code = curl_easy_perform(m_curl.get());

	return response;
}
size_t HttpClient::SharedCallback(char* ptr, size_t size, size_t nmemb, void* userdata) {
	size_t totalSize = size * nmemb;
	std::string* response_body = static_cast<std::string*>(userdata);
	response_body->append(ptr, totalSize);
	return totalSize;
}
