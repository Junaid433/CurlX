#include "CurlX/Response.hpp"

namespace CurlX {

// Copy constructor
RESPONSE::RESPONSE(const RESPONSE& other)
    : statusCode(other.statusCode)
    , reason(other.reason)
    , url(other.url)
    , is_redirect(other.is_redirect)
    , headers(other.headers)
    , body(other.body)
    , request_url(other.request_url)
    , request_headers(other.request_headers)
    , received_cookies(other.received_cookies)
    , elapsed_time(other.elapsed_time)
    , history(other.history)
    , timestamp(other.timestamp)
    , content_length(other.content_length)
    , content_type(other.content_type)
    , encoding(other.encoding)
    , is_compressed(other.is_compressed)
    , server_info(other.server_info)
    , last_modified(other.last_modified)
    , etag(other.etag)
    , cached_json_(other.cached_json_)
    , content_type_detected_(other.content_type_detected_)
    , optimized_(other.optimized_) {}

// Move constructor
RESPONSE::RESPONSE(RESPONSE&& other) noexcept
    : statusCode(other.statusCode)
    , reason(std::move(other.reason))
    , url(std::move(other.url))
    , is_redirect(other.is_redirect)
    , headers(std::move(other.headers))
    , body(std::move(other.body))
    , request_url(std::move(other.request_url))
    , request_headers(std::move(other.request_headers))
    , received_cookies(std::move(other.received_cookies))
    , elapsed_time(other.elapsed_time)
    , history(std::move(other.history))
    , timestamp(other.timestamp)
    , content_length(other.content_length)
    , content_type(std::move(other.content_type))
    , encoding(std::move(other.encoding))
    , is_compressed(other.is_compressed)
    , server_info(std::move(other.server_info))
    , last_modified(std::move(other.last_modified))
    , etag(std::move(other.etag))
    , cached_json_(std::move(other.cached_json_))
    , content_type_detected_(other.content_type_detected_)
    , optimized_(other.optimized_) {
    
    // Reset the moved-from object
    other.statusCode = 0;
    other.is_redirect = false;
    other.elapsed_time = 0.0;
    other.content_length = 0;
    other.is_compressed = false;
    other.content_type_detected_ = false;
    other.optimized_ = false;
}

// Copy assignment operator
RESPONSE& RESPONSE::operator=(const RESPONSE& other) {
    if (this != &other) {
        statusCode = other.statusCode;
        reason = other.reason;
        url = other.url;
        is_redirect = other.is_redirect;
        headers = other.headers;
        body = other.body;
        request_url = other.request_url;
        request_headers = other.request_headers;
        received_cookies = other.received_cookies;
        elapsed_time = other.elapsed_time;
        history = other.history;
        timestamp = other.timestamp;
        content_length = other.content_length;
        content_type = other.content_type;
        encoding = other.encoding;
        is_compressed = other.is_compressed;
        server_info = other.server_info;
        last_modified = other.last_modified;
        etag = other.etag;
        cached_json_ = other.cached_json_;
        content_type_detected_ = other.content_type_detected_;
        optimized_ = other.optimized_;
    }
    return *this;
}

// Move assignment operator
RESPONSE& RESPONSE::operator=(RESPONSE&& other) noexcept {
    if (this != &other) {
        statusCode = other.statusCode;
        reason = std::move(other.reason);
        url = std::move(other.url);
        is_redirect = other.is_redirect;
        headers = std::move(other.headers);
        body = std::move(other.body);
        request_url = std::move(other.request_url);
        request_headers = std::move(other.request_headers);
        received_cookies = std::move(other.received_cookies);
        elapsed_time = other.elapsed_time;
        history = std::move(other.history);
        timestamp = other.timestamp;
        content_length = other.content_length;
        content_type = std::move(other.content_type);
        encoding = std::move(other.encoding);
        is_compressed = other.is_compressed;
        server_info = std::move(other.server_info);
        last_modified = std::move(other.last_modified);
        etag = std::move(other.etag);
        cached_json_ = std::move(other.cached_json_);
        content_type_detected_ = other.content_type_detected_;
        optimized_ = other.optimized_;
        
        // Reset the moved-from object
        other.statusCode = 0;
        other.is_redirect = false;
        other.elapsed_time = 0.0;
        other.content_length = 0;
        other.is_compressed = false;
        other.content_type_detected_ = false;
        other.optimized_ = false;
    }
    return *this;
}

// Basic validation method
bool RESPONSE::is_valid() const noexcept {
    try {
        return statusCode >= 0 && statusCode < 1000;
    } catch (...) {
        return false;
    }
}

// Basic status checking methods
bool RESPONSE::ok() const noexcept {
    return statusCode >= 200 && statusCode < 300;
}

bool RESPONSE::is_client_error() const noexcept {
    return statusCode >= 400 && statusCode < 500;
}

bool RESPONSE::is_server_error() const noexcept {
    return statusCode >= 500 && statusCode < 600;
}

bool RESPONSE::is_redirect_status() const noexcept {
    return statusCode >= 300 && statusCode < 400;
}

bool RESPONSE::is_informational() const noexcept {
    return statusCode >= 100 && statusCode < 200;
}

bool RESPONSE::is_success() const noexcept {
    return statusCode >= 200 && statusCode < 300;
}

// Basic content access methods
std::string RESPONSE::text() const noexcept {
    return body;
}

std::string_view RESPONSE::text_view() const noexcept {
    return body;
}

// Basic utility methods
bool RESPONSE::is_empty() const noexcept {
    return body.empty();
}

bool RESPONSE::is_binary() const noexcept {
    return !body.empty() && (content_type.find("text/") == std::string::npos);
}

size_t RESPONSE::get_content_size() const noexcept {
    return body.length();
}

std::string RESPONSE::get_content_type() const noexcept {
    return content_type;
}

double RESPONSE::get_response_time() const noexcept {
    return elapsed_time;
}

} // namespace CurlX
