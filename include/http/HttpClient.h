#pragma once

#include <string>
#include <map>
#include <memory>
#include <format>
#include <curl/curl.h>

typedef std::map<std::string, std::string> StringMap;
struct Response {
    CURLcode code = CURLcode::CURLE_OK;
    std::string headers;
    std::string body;
};
using CurlUniquePtr = std::unique_ptr<CURL, void(*)(CURL*)>;
using CurlSlistUniquePtr = std::unique_ptr<curl_slist, void(*)(curl_slist*)>;
class HttpClient {
private:
    CurlUniquePtr m_curl;
	StringMap m_cookies;

    static size_t SharedCallback(char* ptr, size_t size, size_t nmemb, void* userdata);
public:
    HttpClient();
   
	Response get(const std::string& url, const StringMap& headers = {});
	Response post(const std::string& url, const StringMap& data = {}, const StringMap& headers = {});
	void setCookie(const std::string& name, const std::string& value);
	StringMap getCookies() const;
    std::string getCookieString() const;

};