#include "CurlX/Session.hpp"
#include "CurlX/Response.hpp"
#include "CurlX/Request.hpp"
#include "CurlX/Headers.hpp"
#include "CurlX/Params.hpp"
#include "CurlX/Files.hpp"
#include "CurlX/Cookies.hpp"
#include "CurlX/Auth.hpp"
#include "CurlX/Exceptions.hpp"
#include <curl/curl.h>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <utility>
#include <cstdio>
#include <algorithm>
#include <chrono>
#include <thread>
#include <future>
#include <memory>
#include <cassert>

namespace CurlX {

// Enhanced helper functions with safety checks
namespace {
    // Safe URL encoding with bounds checking
    std::string safe_url_encode(std::string_view value) {
        if (value.empty()) return {};
        
        std::ostringstream escaped;
        escaped.fill('0');
        escaped << std::hex;

        for (unsigned char c : value) {
            if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
                escaped << static_cast<char>(c);
            } else {
                escaped << '%' << std::setw(2) << static_cast<int>(c);
            }
        }
        return escaped.str();
    }

    // Enhanced write callback with safety checks
    size_t safe_write_callback(void* contents, size_t size, size_t nmemb, std::string* s) noexcept {
        if (!contents || !s || size == 0 || nmemb == 0) {
            return 0;
        }
        
        try {
            const size_t new_length = size * nmemb;
            // Prevent excessive memory allocation
            if (new_length > 100 * 1024 * 1024) { // 100MB limit
                return 0;
            }
            s->append(static_cast<const char*>(contents), new_length);
            return new_length;
        } catch (const std::exception&) {
            return 0;
        }
    }

    // Safe file write callback
    size_t safe_file_write_callback(void* contents, size_t size, size_t nmemb, FILE* stream) noexcept {
        if (!contents || !stream || size == 0 || nmemb == 0) {
            return 0;
        }
        
        const size_t result = std::fwrite(contents, size, nmemb, stream);
        if (std::ferror(stream)) {
            std::clearerr(stream);
            return 0;
        }
        return result;
    }

    // Enhanced header callback with validation
    size_t safe_header_callback(char* buffer, size_t size, size_t nitems, HEADERS* headers) noexcept {
        if (!buffer || !headers || size == 0 || nitems == 0) {
            return 0;
        }
        
        try {
            const std::string header(buffer, size * nitems);
            if (header.length() > 2 && header.length() < 8192) { // Reasonable header size limit
                // Remove \r\n safely
                const std::string clean_header = header.substr(0, header.length() - 2);
                headers->add(clean_header);
            }
            return size * nitems;
        } catch (const std::exception&) {
            return 0;
        }
    }

    // Safe string operations
    template<typename T>
    bool safe_string_operation(const std::function<void()>& operation) noexcept {
        try {
            operation();
            return true;
        } catch (const std::exception&) {
            return false;
        }
    }
}

// SessionPool implementation
SessionPool::SessionPool(size_t max_size) : max_size_(max_size) {
    if (max_size_ == 0) max_size_ = 1;
}

SessionPool::~SessionPool() = default;

std::shared_ptr<Session> SessionPool::acquire_session() {
    std::unique_lock<std::mutex> lock(pool_mutex_);
    
    // Wait for available session
    pool_cv_.wait(lock, [this] { return !available_sessions_.empty() || in_use_sessions_.size() < max_size_; });
    
    if (!available_sessions_.empty()) {
        auto session = available_sessions_.back();
        available_sessions_.pop_back();
        in_use_sessions_.push_back(session);
        return session;
    } else if (in_use_sessions_.size() < max_size_) {
        auto session = std::make_shared<Session>(false); // Don't enable pooling to avoid recursion
        in_use_sessions_.push_back(session);
        return session;
    }
    
    return nullptr;
}

void SessionPool::release_session(std::shared_ptr<Session> session) {
    if (!session) return;
    
    std::lock_guard<std::mutex> lock(pool_mutex_);
    
    // Remove from in-use list
    auto it = std::find(in_use_sessions_.begin(), in_use_sessions_.end(), session);
    if (it != in_use_sessions_.end()) {
        in_use_sessions_.erase(it);
        
        // Reset session for reuse
        session->reset();
        
        // Add back to available pool if not at capacity
        if (available_sessions_.size() < max_size_) {
            available_sessions_.push_back(session);
        }
    }
}

size_t SessionPool::size() const noexcept {
    std::lock_guard<std::mutex> lock(pool_mutex_);
    return available_sessions_.size() + in_use_sessions_.size();
}

size_t SessionPool::available() const noexcept {
    std::lock_guard<std::mutex> lock(pool_mutex_);
    return available_sessions_.size();
}

// Session implementation
Session::Session(bool enable_connection_pooling) 
    : pooling_enabled_(enable_connection_pooling) {
    initialize_curl_handle();
}

Session::Session(Session&& other) noexcept 
    : curl_handle_(std::move(other.curl_handle_))
    , default_headers_(std::move(other.default_headers_))
    , default_cookies_(std::move(other.default_cookies_))
    , cookie_jar_path_(std::move(other.cookie_jar_path_))
    , request_count_(other.request_count_.load())
    , total_response_time_(other.total_response_time_.load())
    , connection_pool_(std::move(other.connection_pool_))
    , pooling_enabled_(other.pooling_enabled_)
    , is_valid_(other.is_valid_.load())
    , connection_timeout_(other.connection_timeout_)
    , transfer_timeout_(other.transfer_timeout_)
    , max_connections_per_host_(other.max_connections_per_host_)
    , keep_alive_enabled_(other.keep_alive_enabled_)
    , compression_enabled_(other.compression_enabled_) {
    
    other.is_valid_.store(false);
    other.request_count_.store(0);
    other.total_response_time_.store(0.0);
}

Session& Session::operator=(Session&& other) noexcept {
    if (this != &other) {
        cleanup_curl_handle();
        
        curl_handle_ = std::move(other.curl_handle_);
        default_headers_ = std::move(other.default_headers_);
        default_cookies_ = std::move(other.default_cookies_);
        cookie_jar_path_ = std::move(other.cookie_jar_path_);
        request_count_.store(other.request_count_.load());
        total_response_time_.store(other.total_response_time_.load());
        connection_pool_ = std::move(other.connection_pool_);
        pooling_enabled_ = other.pooling_enabled_;
        is_valid_.store(other.is_valid_.load());
        connection_timeout_ = other.connection_timeout_;
        transfer_timeout_ = other.transfer_timeout_;
        max_connections_per_host_ = other.max_connections_per_host_;
        keep_alive_enabled_ = other.keep_alive_enabled_;
        compression_enabled_ = other.compression_enabled_;
        
        other.is_valid_.store(false);
        other.request_count_.store(0);
        other.total_response_time_.store(0.0);
    }
    return *this;
}

Session::~Session() {
    cleanup_curl_handle();
}

void Session::initialize_curl_handle() {
    CURL* handle = curl_easy_init();
    if (!handle) {
        throw RequestException("Failed to initialize CURL handle");
    }
    
    // Use RAII wrapper with custom deleter
    curl_handle_ = std::unique_ptr<CURL, CurlHandleDeleter>(handle);
    
    // Apply default settings
    apply_safety_settings();
    apply_performance_settings();
    
    // Enable cookie engine by default (in-memory)
    curl_easy_setopt(handle, CURLOPT_COOKIEFILE, "");
    
    // Enable automatic content decoding
    curl_easy_setopt(handle, CURLOPT_ACCEPT_ENCODING, "");
    
    is_valid_.store(true);
}

void Session::cleanup_curl_handle() noexcept {
    if (curl_handle_) {
        // Save cookies if jar path is set
        if (!cookie_jar_path_.empty()) {
            try {
                curl_easy_setopt(curl_handle_.get(), CURLOPT_COOKIEJAR, cookie_jar_path_.c_str());
            } catch (...) {
                // Ignore errors during cleanup
            }
        }
        curl_handle_.reset();
    }
    is_valid_.store(false);
}

void Session::apply_safety_settings() {
    if (!curl_handle_) return;
    
    CURL* handle = curl_handle_.get();
    
    // Set reasonable limits to prevent resource exhaustion
    curl_easy_setopt(handle, CURLOPT_MAXFILESIZE_LARGE, static_cast<curl_off_t>(100 * 1024 * 1024)); // 100MB
    curl_easy_setopt(handle, CURLOPT_BUFFERSIZE, 16384); // 16KB buffer
    curl_easy_setopt(handle, CURLOPT_MAXREDIRS, 10); // Limit redirects
    
    // Security settings - use modern CURL options
    curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 2L);
    
    // Use modern protocol options instead of deprecated ones
    #if LIBCURL_VERSION_NUM >= 0x075500 // 7.85.0
        curl_easy_setopt(handle, CURLOPT_PROTOCOLS_STR, "http,https");
        curl_easy_setopt(handle, CURLOPT_REDIR_PROTOCOLS_STR, "http,https");
    #else
        curl_easy_setopt(handle, CURLOPT_PROTOCOLS, CURLPROTO_HTTP | CURLPROTO_HTTPS);
        curl_easy_setopt(handle, CURLOPT_REDIR_PROTOCOLS, CURLPROTO_HTTP | CURLPROTO_HTTPS);
    #endif
}

void Session::apply_performance_settings() {
    if (!curl_handle_) return;
    
    CURL* handle = curl_handle_.get();
    
    // Connection settings
    curl_easy_setopt(handle, CURLOPT_CONNECTTIMEOUT, static_cast<long>(connection_timeout_));
    curl_easy_setopt(handle, CURLOPT_TIMEOUT, static_cast<long>(transfer_timeout_));
    curl_easy_setopt(handle, CURLOPT_TCP_KEEPALIVE, keep_alive_enabled_ ? 1L : 0L);
    curl_easy_setopt(handle, CURLOPT_TCP_KEEPIDLE, 60L);
    curl_easy_setopt(handle, CURLOPT_TCP_KEEPINTVL, 60L);
    
    // Connection pooling
    curl_easy_setopt(handle, CURLOPT_MAXCONNECTS, static_cast<long>(max_connections_per_host_));
    curl_easy_setopt(handle, CURLOPT_FORBID_REUSE, 0L);
    curl_easy_setopt(handle, CURLOPT_FRESH_CONNECT, 0L);
    
    // Compression
    if (compression_enabled_) {
        curl_easy_setopt(handle, CURLOPT_ACCEPT_ENCODING, "gzip,deflate");
    }
    
    // DNS caching
    curl_easy_setopt(handle, CURLOPT_DNS_CACHE_TIMEOUT, 300L); // 5 minutes
}

void Session::validate_request(const REQUEST& request) const {
    if (!is_valid_.load()) {
        throw RequestException("Session is not valid");
    }
    
    // Basic URL validation - check if URL is not empty
    if (request.get_url().toString().empty()) {
        throw RequestException("Invalid URL in request");
    }
    
    // Validate file paths if present
    if (!request.get_output_file_path().empty()) {
        // Check if directory is writable
        std::string dir = request.get_output_file_path();
        size_t last_slash = dir.find_last_of("/\\");
        if (last_slash != std::string::npos) {
            dir = dir.substr(0, last_slash);
            if (!dir.empty() && access(dir.c_str(), W_OK) != 0) {
                throw RequestException("Output directory is not writable: " + dir);
            }
        }
    }
}

RESPONSE Session::send(const REQUEST& request) {
    const auto start_time = std::chrono::high_resolution_clock::now();
    
    try {
        // Validate request
        validate_request(request);
        
        // Acquire lock for thread safety
        std::lock_guard<std::mutex> lock(session_mutex_);
        
        if (!curl_handle_) {
            throw RequestException("CURL handle is not available");
        }
        
        CURL* handle = curl_handle_.get();
        CURLcode res;
        RESPONSE response;
        std::string response_body;
        HEADERS response_headers;
        curl_mime* mime = nullptr;
        FILE* output_file = nullptr;
        
        // Reset options for each request
        curl_easy_reset(handle);
        
        // Reapply settings
        apply_safety_settings();
        apply_performance_settings();
        
        // Build full URL with parameters
        std::string full_url = request.get_url().toString();
        if (!request.get_params().get().empty()) {
            full_url += "?";
            bool first_param = true;
            for (const auto& pair : request.get_params().get()) {
                if (!first_param) full_url += "&";
                full_url += safe_url_encode(pair.first) + "=" + safe_url_encode(pair.second);
                first_param = false;
            }
        }
        
        curl_easy_setopt(handle, CURLOPT_URL, full_url.c_str());
        curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, request.get_method().c_str());
        
        // Handle file uploads
        if (!request.files_.get().empty()) {
            mime = curl_mime_init(handle);
            if (!mime) {
                throw RequestException("Failed to initialize MIME structure");
            }
            
            for (const auto& file_pair : request.files_.get()) {
                curl_mimepart* part = curl_mime_addpart(mime);
                if (!part) continue;
                
                curl_mime_name(part, file_pair.first.c_str());
                curl_mime_filedata(part, file_pair.second.c_str());
            }
            
            // Add form fields
            for (const auto& field_pair : request.params_.get()) {
                curl_mimepart* part = curl_mime_addpart(mime);
                if (!part) continue;
                
                curl_mime_name(part, field_pair.first.c_str());
                curl_mime_data(part, field_pair.second.c_str(), CURL_ZERO_TERMINATED);
            }
            
            curl_easy_setopt(handle, CURLOPT_MIMEPOST, mime);
        } else if (!request.body_.toString().empty()) {
            curl_easy_setopt(handle, CURLOPT_POSTFIELDS, request.body_.c_str());
            curl_easy_setopt(handle, CURLOPT_POSTFIELDSIZE, request.body_.length());
        }
        
        // Handle output
        if (!request.output_file_path_.empty()) {
            output_file = fopen(request.output_file_path_.c_str(), "wb");
            if (!output_file) {
                throw RequestException("Failed to open output file: " + request.output_file_path_);
            }
            curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, safe_file_write_callback);
            curl_easy_setopt(handle, CURLOPT_WRITEDATA, output_file);
        } else if (request.method_.value == "HEAD") {
            curl_easy_setopt(handle, CURLOPT_NOBODY, 1L);
            curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, nullptr);
            curl_easy_setopt(handle, CURLOPT_WRITEDATA, nullptr);
        } else {
            curl_easy_setopt(handle, CURLOPT_NOBODY, 0L);
            curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, safe_write_callback);
            curl_easy_setopt(handle, CURLOPT_WRITEDATA, &response_body);
        }
        
        // Set headers
        curl_easy_setopt(handle, CURLOPT_HEADERFUNCTION, safe_header_callback);
        curl_easy_setopt(handle, CURLOPT_HEADERDATA, &response_headers);
        
        // Merge headers
        HEADERS effective_headers = default_headers_;
        for (const auto& header_line : request.headers_.all()) {
            effective_headers.add(header_line);
        }
        
        struct curl_slist* chunk = effective_headers.to_curl_slist();
        if (chunk) {
            curl_easy_setopt(handle, CURLOPT_HTTPHEADER, chunk);
        }
        
        // Handle cookies
        COOKIES effective_cookies = default_cookies_;
        for (const auto& cookie_pair : request.cookies_.all()) {
            effective_cookies.add(cookie_pair.first, cookie_pair.second);
        }
        
        for (const auto& cookie_pair : effective_cookies.all()) {
            std::string cookie_string = cookie_pair.first + "=" + cookie_pair.second;
            curl_easy_setopt(handle, CURLOPT_COOKIELIST, cookie_string.c_str());
        }
        
        // Handle authentication
        if (request.auth_.type() != AuthType::None) {
            curl_easy_setopt(handle, CURLOPT_USERPWD, request.auth_.user_pass_string().c_str());
            if (request.auth_.type() == AuthType::Basic) {
                curl_easy_setopt(handle, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
            } else if (request.auth_.type() == AuthType::Digest) {
                curl_easy_setopt(handle, CURLOPT_HTTPAUTH, CURLAUTH_DIGEST);
            }
        }
        
        // Handle redirects
        if (request.allow_redirects_.allow()) {
            curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(handle, CURLOPT_MAXREDIRS, request.allow_redirects_.getMaxRedirects());
        } else {
            curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 0L);
        }
        
        // Execute request
        res = curl_easy_perform(handle);
        
        if (res == CURLE_OK) {
            // Get response information
            long response_code = 0;
            curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &response_code);
            
            response.statusCode = response_code;
            response.body = response_body;
            response.headers = response_headers;
            response.request_url = request.url_;
            response.request_headers = effective_headers;
            
            // Get timing information
            double total_time = 0.0;
            curl_easy_getinfo(handle, CURLINFO_TOTAL_TIME, &total_time);
            response.elapsed_time = total_time;
            
            // Parse received cookies
            for (const auto& header_line : response_headers.all()) {
                if (header_line.rfind("Set-Cookie:", 0) == 0) {
                    std::string cookie_str = header_line.substr(12); // Skip "Set-Cookie: "
                    size_t eq_pos = cookie_str.find('=');
                    if (eq_pos != std::string::npos) {
                        std::string cookie_name = cookie_str.substr(0, eq_pos);
                        size_t semicolon_pos = cookie_str.find(';', eq_pos);
                        std::string cookie_value = cookie_str.substr(eq_pos + 1, 
                            semicolon_pos != std::string::npos ? semicolon_pos - (eq_pos + 1) : std::string::npos);
                        response.received_cookies.add(cookie_name, cookie_value);
                    }
                }
            }
            
            // Build redirect history
            long redirect_count = 0;
            curl_easy_getinfo(handle, CURLINFO_REDIRECT_COUNT, &redirect_count);
            if (redirect_count > 0) {
                char* effective_url_cstr = nullptr;
                curl_easy_getinfo(handle, CURLINFO_EFFECTIVE_URL, &effective_url_cstr);
                if (effective_url_cstr) {
                    response.history.push_back(URL(effective_url_cstr));
                }
            }
            
            // Update statistics
            const auto end_time = std::chrono::high_resolution_clock::now();
            const auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
            update_statistics(duration.count() / 1000000.0);
            
        } else {
            // Handle CURL errors
            std::string error_message = curl_easy_strerror(res);
            switch (res) {
                case CURLE_COULDNT_CONNECT:
                case CURLE_COULDNT_RESOLVE_HOST:
                    throw ConnectionError(error_message);
                case CURLE_OPERATION_TIMEDOUT:
                    throw Timeout(error_message);
                case CURLE_TOO_MANY_REDIRECTS:
                    throw TooManyRedirects(error_message);
                default:
                    throw RequestException(error_message);
            }
        }
        
        // Cleanup
        if (chunk) curl_slist_free_all(chunk);
        if (mime) curl_mime_free(mime);
        if (output_file) fclose(output_file);
        
        return response;
        
    } catch (const std::exception& e) {
        // Update statistics even on failure
        const auto end_time = std::chrono::high_resolution_clock::now();
        const auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        update_statistics(duration.count() / 1000000.0);
        throw;
    }
}

std::future<CurlX::RESPONSE> Session::send_async(const REQUEST& request) {
    return std::async(std::launch::async, [this, request]() {
        return this->send(request);
    });
}

// HTTP verb implementations
RESPONSE Session::GET(const URL& url, const PARAMS& params, const HEADERS& headers, 
                      const COOKIES& cookies, const TIMEOUT& timeout, const AUTH& auth, 
                      const PROXY& proxy, const REDIRECTS& redirects, const VERIFY& verify) {
    REQUEST request(url, METHOD::GET, headers, BODY(), timeout, auth, proxy, cookies, redirects, verify, params);
    return send(request);
}

RESPONSE Session::POST(const URL& url, const BODY& body, const HEADERS& headers, 
                       const COOKIES& cookies, const TIMEOUT& timeout, const AUTH& auth, 
                       const PROXY& proxy, const REDIRECTS& redirects, const VERIFY& verify, 
                       const FILES& files, const PARAMS& params) {
    REQUEST request(url, METHOD::POST, headers, body, timeout, auth, proxy, cookies, redirects, verify, params, files);
    return send(request);
}

RESPONSE Session::PUT(const URL& url, const BODY& body, const HEADERS& headers, 
                      const COOKIES& cookies, const TIMEOUT& timeout, const AUTH& auth, 
                      const PROXY& proxy, const REDIRECTS& redirects, const VERIFY& verify) {
    REQUEST request(url, METHOD::PUT, headers, body, timeout, auth, proxy, cookies, redirects, verify);
    return send(request);
}

RESPONSE Session::DELETE(const URL& url, const HEADERS& headers, const COOKIES& cookies, 
                         const TIMEOUT& timeout, const AUTH& auth, const PROXY& proxy, 
                         const REDIRECTS& redirects, const VERIFY& verify) {
    REQUEST request(url, METHOD::DELETE, headers, BODY(), timeout, auth, proxy, cookies, redirects, verify);
    return send(request);
}

RESPONSE Session::PATCH(const URL& url, const BODY& body, const HEADERS& headers, 
                        const COOKIES& cookies, const TIMEOUT& timeout, const AUTH& auth, 
                        const PROXY& proxy, const REDIRECTS& redirects, const VERIFY& verify) {
    REQUEST request(url, METHOD::PATCH, headers, body, timeout, auth, proxy, cookies, redirects, verify);
    return send(request);
}

RESPONSE Session::HEAD(const URL& url, const HEADERS& headers, const COOKIES& cookies, 
                       const TIMEOUT& timeout, const AUTH& auth, const PROXY& proxy, 
                       const REDIRECTS& redirects, const VERIFY& verify) {
    REQUEST request(url, METHOD::HEAD, headers, BODY(), timeout, auth, proxy, cookies, redirects, verify);
    return send(request);
}

RESPONSE Session::OPTIONS(const URL& url, const HEADERS& headers, const COOKIES& cookies, 
                          const TIMEOUT& timeout, const AUTH& auth, const PROXY& proxy, 
                          const REDIRECTS& redirects, const VERIFY& verify) {
    REQUEST request(url, METHOD::OPTIONS, headers, BODY(), timeout, auth, proxy, cookies, redirects, verify);
    return send(request);
}

// Configuration methods
void Session::set_default_headers(const HEADERS& headers) {
    std::lock_guard<std::mutex> lock(session_mutex_);
    default_headers_ = headers;
}

void Session::set_default_cookies(const COOKIES& cookies) {
    std::lock_guard<std::mutex> lock(session_mutex_);
    default_cookies_ = cookies;
}

void Session::set_cookie_jar(const std::string& file_path) {
    std::lock_guard<std::mutex> lock(session_mutex_);
    cookie_jar_path_ = file_path;
    if (curl_handle_) {
        curl_easy_setopt(curl_handle_.get(), CURLOPT_COOKIEFILE, cookie_jar_path_.c_str());
        curl_easy_setopt(curl_handle_.get(), CURLOPT_COOKIEJAR, cookie_jar_path_.c_str());
    }
}

// Performance tuning methods
void Session::set_connection_timeout(double seconds) {
    if (seconds < 0.0) seconds = 0.0;
    connection_timeout_ = seconds;
    if (curl_handle_) {
        curl_easy_setopt(curl_handle_.get(), CURLOPT_CONNECTTIMEOUT, static_cast<long>(seconds));
    }
}

void Session::set_transfer_timeout(double seconds) {
    if (seconds < 0.0) seconds = 0.0;
    transfer_timeout_ = seconds;
    if (curl_handle_) {
        curl_easy_setopt(curl_handle_.get(), CURLOPT_TIMEOUT, static_cast<long>(seconds));
    }
}

void Session::set_max_connections_per_host(size_t max_conns) {
    max_connections_per_host_ = max_conns;
    if (curl_handle_) {
        curl_easy_setopt(curl_handle_.get(), CURLOPT_MAXCONNECTS, static_cast<long>(max_conns));
    }
}

void Session::set_keep_alive(bool enable) {
    keep_alive_enabled_ = enable;
    if (curl_handle_) {
        curl_easy_setopt(curl_handle_.get(), CURLOPT_TCP_KEEPALIVE, enable ? 1L : 0L);
    }
}

void Session::set_compression(bool enable) {
    compression_enabled_ = enable;
    if (curl_handle_) {
        curl_easy_setopt(curl_handle_.get(), CURLOPT_ACCEPT_ENCODING, enable ? "gzip,deflate" : "");
    }
}

// Safety and monitoring methods
bool Session::is_valid() const noexcept {
    return is_valid_.load() && curl_handle_ != nullptr;
}

void Session::reset() noexcept {
    std::lock_guard<std::mutex> lock(session_mutex_);
    if (curl_handle_) {
        curl_easy_reset(curl_handle_.get());
        apply_safety_settings();
        apply_performance_settings();
    }
}

size_t Session::get_request_count() const noexcept {
    return request_count_.load();
}

double Session::get_average_response_time() const noexcept {
    std::lock_guard<std::mutex> lock(stats_mutex_);
    size_t count = request_count_.load();
    return count > 0 ? total_response_time_.load() / count : 0.0;
}

// Connection pooling
void Session::enable_connection_pooling(bool enable) {
    pooling_enabled_ = enable;
}

void Session::set_pool_size(size_t size) {
    (void)size; // Suppress unused parameter warning
    if (connection_pool_) {
        // Note: This would require a more sophisticated pool implementation
        // For now, we'll just store the preference
    }
}

CURL* Session::get_curl_handle() const noexcept {
    return curl_handle_ ? curl_handle_.get() : nullptr;
}

// Private helper methods
void Session::update_statistics(double response_time) {
    request_count_.fetch_add(1);
    total_response_time_.fetch_add(response_time);
}

// CurlHandleDeleter implementation
void Session::CurlHandleDeleter::operator()(CURL* handle) const noexcept {
    if (handle) {
        curl_easy_cleanup(handle);
    }
}

// Template method implementation
template<typename Func>
auto Session::with_lock(Func&& func) const -> decltype(func()) {
    std::lock_guard<std::mutex> lock(session_mutex_);
    return func();
}

} // namespace CurlX
