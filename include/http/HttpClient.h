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

    std::string m_cookieString;
    CurlSlistUniquePtr m_headerList;

    void getCookieString(std::string& outCookieString) const;
    bool SetupRequest(const std::string& url, const StringMap& headers, Response& response);

    static size_t SharedCallback(char* ptr, size_t size, size_t nmemb, void* userdata);
public:
    HttpClient();

    Response get(const std::string& url, const StringMap& headers = {});
    Response post(const std::string& url, const std::string& body, const StringMap& headers = {});
    inline void setCookie(const std::string& cookieName, const std::string& cookieValue) {
        m_cookies.insert_or_assign(cookieName, cookieValue);
    };
    inline void clearCookies() {
        m_cookies.clear();
    };
    inline const StringMap getCookies() const {
        return m_cookies;
    };
};