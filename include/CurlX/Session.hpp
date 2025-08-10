#pragma once

#include "Response.hpp"
#include "Url.hpp"
#include "Headers.hpp"
#include "Params.hpp"
#include "Cookies.hpp"
#include "Timeout.hpp"
#include "Auth.hpp"
#include "Proxy.hpp"
#include "Redirects.hpp"
#include "Verify.hpp"
#include "Body.hpp"
#include "Files.hpp"
#include <curl/curl.h>

namespace CurlX {

// Forward declaration to break circular dependency
struct REQUEST;

class Session {
public:
    Session();
    ~Session();

    CurlX::RESPONSE send(const REQUEST& request);

    // New member functions for HTTP verbs
    RESPONSE GET(const URL& url, const PARAMS& params = PARAMS(), const HEADERS& headers = HEADERS(), const COOKIES& cookies = COOKIES(), const TIMEOUT& timeout = TIMEOUT(), const AUTH& auth = AUTH(), const PROXY& proxy = PROXY(), const REDIRECTS& redirects = REDIRECTS(), const VERIFY& verify = VERIFY());
    RESPONSE POST(const URL& url, const BODY& body = BODY(), const HEADERS& headers = HEADERS(), const COOKIES& cookies = COOKIES(), const TIMEOUT& timeout = TIMEOUT(), const AUTH& auth = AUTH(), const PROXY& proxy = PROXY(), const REDIRECTS& redirects = REDIRECTS(), const VERIFY& verify = VERIFY(), const FILES& files = FILES(), const PARAMS& params = PARAMS());
    RESPONSE PUT(const URL& url, const BODY& body = BODY(), const HEADERS& headers = HEADERS(), const COOKIES& cookies = COOKIES(), const TIMEOUT& timeout = TIMEOUT(), const AUTH& auth = AUTH(), const PROXY& proxy = PROXY(), const REDIRECTS& redirects = REDIRECTS(), const VERIFY& verify = VERIFY());
    RESPONSE DELETE(const URL& url, const HEADERS& headers = HEADERS(), const COOKIES& cookies = COOKIES(), const TIMEOUT& timeout = TIMEOUT(), const AUTH& auth = AUTH(), const PROXY& proxy = PROXY(), const REDIRECTS& redirects = REDIRECTS(), const VERIFY& verify = VERIFY());
    RESPONSE PATCH(const URL& url, const BODY& body = BODY(), const HEADERS& headers = HEADERS(), const COOKIES& cookies = COOKIES(), const TIMEOUT& timeout = TIMEOUT(), const AUTH& auth = AUTH(), const PROXY& proxy = PROXY(), const REDIRECTS& redirects = REDIRECTS(), const VERIFY& verify = VERIFY());
    RESPONSE HEAD(const URL& url, const HEADERS& headers = HEADERS(), const COOKIES& cookies = COOKIES(), const TIMEOUT& timeout = TIMEOUT(), const AUTH& auth = AUTH(), const PROXY& proxy = PROXY(), const REDIRECTS& redirects = REDIRECTS(), const VERIFY& verify = VERIFY());
    RESPONSE OPTIONS(const URL& url, const HEADERS& headers = HEADERS(), const COOKIES& cookies = COOKIES(), const TIMEOUT& timeout = TIMEOUT(), const AUTH& auth = AUTH(), const PROXY& proxy = PROXY(), const REDIRECTS& redirects = REDIRECTS(), const VERIFY& verify = VERIFY());

    void set_default_headers(const HEADERS& headers);
    void set_default_cookies(const COOKIES& cookies);

    // Cookie persistence
    void set_cookie_jar(const std::string& file_path);

    CURL* get_curl_handle();

private:
    CURL* curl_handle;
    HEADERS default_headers;
    COOKIES default_cookies;
    std::string cookie_jar_path;
};

} // namespace CurlX
