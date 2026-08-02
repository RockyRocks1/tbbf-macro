#pragma once
#include <sstream>
#include <string>
#include <map>
#include <memory>
#include <format>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

struct CaseInsensitiveLess {
    bool operator()(const std::string& lhs, const std::string& rhs) const {
        return std::lexicographical_compare(
            lhs.begin(), lhs.end(),
            rhs.begin(), rhs.end(),
            [](unsigned char left, unsigned char right) {
                return std::tolower(left) < std::tolower(right);
            }
        );
    }
};
using HeaderMap = std::map<std::string, std::string, CaseInsensitiveLess>;
using StringMap = std::map<std::string, std::string> ;

struct HttpResponse {
    CURLcode code = CURLcode::CURLE_OK;
    long statusCode = 0;
    HeaderMap headers;
    std::string body;
    std::string rawHeaders;

    bool isJson() const {
        if (!headers.contains("Content-Type"))
            return false;
        const std::string& contentType = headers.at("Content-Type");
        return contentType.find("application/json") != std::string::npos;  // TODO: make this case-insensitive
    }
    nlohmann::json json() const {
        return nlohmann::json::parse(body);
    }
};

using CurlUniquePtr = std::unique_ptr<CURL, void(*)(CURL*)>;
using CurlSlistUniquePtr = std::unique_ptr<curl_slist, void(*)(curl_slist*)>;
class HttpClient {
private:
    CurlUniquePtr m_curl;
    StringMap m_cookies;

    // internal variables, dont use
    std::string m_cookieString;
    CurlSlistUniquePtr m_headerList;

    void GetCookieString(std::string& outCookieString) const;
    bool SetupRequest(const std::string& url, const StringMap& headers, HttpResponse& response);
    

    static void FormatRawHeaders(std::string& rawHeader, HeaderMap& headerMap);
    static size_t SharedCallback(char* ptr, size_t size, size_t nmemb, void* userdata);
public:
    HttpClient();
    ~HttpClient() = default;

    HttpResponse get(const std::string& url, const StringMap& headers = {});
    HttpResponse post(const std::string& url, const std::string& body, const StringMap& headers = {});
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