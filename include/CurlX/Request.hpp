#pragma once

#include <string>
#include <functional>
#include "Url.hpp"
#include "Headers.hpp"
#include "Body.hpp"
#include "Timeout.hpp"
#include "Auth.hpp"
#include "Proxy.hpp"
#include "Cookies.hpp"
#include "Redirects.hpp"
#include "Verify.hpp"
#include "Response.hpp"
#include "Method.hpp"
#include "Params.hpp"
#include "Files.hpp"

#include "Session.hpp"

namespace CurlX {
    struct REQUEST {
        using WriteCallback = std::function<size_t(void*, size_t, size_t, void*)>;
        using ReadCallback = std::function<size_t(void*, size_t, size_t, void*)>;

        REQUEST() = default;
        explicit REQUEST(const URL& u,
                         METHOD m = METHOD::GET,
                         HEADERS h = HEADERS(),
                         BODY b = BODY(),
                         TIMEOUT t = TIMEOUT(),
                         AUTH au = AUTH(),
                         PROXY p = PROXY(),
                         COOKIES c = COOKIES(),
                         REDIRECTS ar = REDIRECTS(),
                         VERIFY v = VERIFY(),
                         PARAMS prms = {},
                         FILES f = {},
                         std::string ofp = "")
            : url_(u),
              method_(std::move(m)),
              headers_(std::move(h)),
              body_(std::move(b)),
              timeout_(t),
              auth_(std::move(au)),
              proxy_(std::move(p)),
              cookies_(std::move(c)),
              allow_redirects_(ar),
              verify_(v),
              params_(std::move(prms)),
              files_(std::move(f)),
              output_file_path_(std::move(ofp)) {}

        // Chainable setters
        REQUEST& url(const URL& u) { url_ = u; return *this; }
        REQUEST& method(const METHOD& m) { method_ = m; return *this; }
        REQUEST& headers(const HEADERS& h) { headers_ = h; return *this; }
        REQUEST& params(const PARAMS& p) { params_ = p; return *this; }
        REQUEST& cookies(const COOKIES& c) { cookies_ = c; return *this; }
        REQUEST& body(const BODY& b) { body_ = b; return *this; }
        REQUEST& timeout(const TIMEOUT& t) { timeout_ = t; return *this; }
        REQUEST& auth(const AUTH& a) { auth_ = a; return *this; }
        REQUEST& proxy(const PROXY& p) { proxy_ = p; return *this; }
        REQUEST& redirects(const REDIRECTS& r) { allow_redirects_ = r; return *this; }
        REQUEST& verify(const VERIFY& v) { verify_ = v; return *this; }
        REQUEST& files(const FILES& f) { files_ = f; return *this; }
        REQUEST& output_file_path(const std::string& ofp) { output_file_path_ = ofp; return *this; }
        REQUEST& write_callback(WriteCallback cb, void* userdata = nullptr) { write_cb_ = cb; write_userdata_ = userdata; return *this; }
        REQUEST& read_callback(ReadCallback cb, void* userdata = nullptr) { read_cb_ = cb; read_userdata_ = userdata; return *this; }

        // HTTP verbs
        CurlX::RESPONSE GET(CurlX::Session& session);
        CurlX::RESPONSE POST(CurlX::Session& session);
        // Add other HTTP verbs as needed...

        // Accessors for callbacks (for Session to use)
        WriteCallback get_write_callback() const { return write_cb_; }
        void* get_write_userdata() const { return write_userdata_; }
        ReadCallback get_read_callback() const { return read_cb_; }
        void* get_read_userdata() const { return read_userdata_; }

        // Accessors for request data
        const URL& get_url() const { return url_; }
        const METHOD& get_method() const { return method_; }
        const HEADERS& get_headers() const { return headers_; }
        const PARAMS& get_params() const { return params_; }
        const COOKIES& get_cookies() const { return cookies_; }
        const BODY& get_body() const { return body_; }
        const TIMEOUT& get_timeout() const { return timeout_; }
        const AUTH& get_auth() const { return auth_; }
        const PROXY& get_proxy() const { return proxy_; }
        const REDIRECTS& get_redirects() const { return allow_redirects_; }
        const VERIFY& get_verify() const { return verify_; }
        const FILES& get_files() const { return files_; }
        const std::string& get_output_file_path() const { return output_file_path_; }

    // All members are public by default in a struct
        URL url_;
        METHOD method_ = METHOD::GET;
        HEADERS headers_;
        BODY body_;
        TIMEOUT timeout_;
        AUTH auth_;
        PROXY proxy_;
        COOKIES cookies_;
        REDIRECTS allow_redirects_;
        VERIFY verify_;
        PARAMS params_;
        FILES files_;
        std::string output_file_path_;
        WriteCallback write_cb_ = nullptr;
        void* write_userdata_ = nullptr;
        ReadCallback read_cb_ = nullptr;
        void* read_userdata_ = nullptr;
    };
}
