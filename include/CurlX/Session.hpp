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
#include <memory>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <future>
#include <chrono>
#include <functional>

namespace CurlX {

// Forward declaration to break circular dependency
struct REQUEST;

// Thread-safe session pool for connection reuse
class SessionPool;

class Session {
public:
    // Constructor with enhanced safety
    explicit Session(bool enable_connection_pooling = true);
    
    // Move constructor and assignment for performance
    Session(Session&& other) noexcept;
    Session& operator=(Session&& other) noexcept;
    
    // Delete copy constructor/assignment for safety
    Session(const Session&) = delete;
    Session& operator=(const Session&) = delete;
    
    // Destructor with enhanced cleanup
    ~Session();

    // Core request method with enhanced safety
    CurlX::RESPONSE send(const REQUEST& request);
    
    // Async version for non-blocking operations
    std::future<CurlX::RESPONSE> send_async(const REQUEST& request);

    // HTTP verb methods with enhanced error handling
    RESPONSE GET(const URL& url, const PARAMS& params = PARAMS(), const HEADERS& headers = HEADERS(), 
                 const COOKIES& cookies = COOKIES(), const TIMEOUT& timeout = TIMEOUT(), 
                 const AUTH& auth = AUTH(), const PROXY& proxy = PROXY(), 
                 const REDIRECTS& redirects = REDIRECTS(), const VERIFY& verify = VERIFY());
    
    RESPONSE POST(const URL& url, const BODY& body = BODY(), const HEADERS& headers = HEADERS(), 
                  const COOKIES& cookies = COOKIES(), const TIMEOUT& timeout = TIMEOUT(), 
                  const AUTH& auth = AUTH(), const PROXY& proxy = PROXY(), 
                  const REDIRECTS& redirects = REDIRECTS(), const VERIFY& verify = VERIFY(), 
                  const FILES& files = FILES(), const PARAMS& params = PARAMS());
    
    RESPONSE PUT(const URL& url, const BODY& body = BODY(), const HEADERS& headers = HEADERS(), 
                 const COOKIES& cookies = COOKIES(), const TIMEOUT& timeout = TIMEOUT(), 
                 const AUTH& auth = AUTH(), const PROXY& proxy = PROXY(), 
                 const REDIRECTS& redirects = REDIRECTS(), const VERIFY& verify = VERIFY());
    
    RESPONSE DELETE(const URL& url, const HEADERS& headers = HEADERS(), 
                    const COOKIES& cookies = COOKIES(), const TIMEOUT& timeout = TIMEOUT(), 
                    const AUTH& auth = AUTH(), const PROXY& proxy = PROXY(), 
                    const REDIRECTS& redirects = REDIRECTS(), const VERIFY& verify = VERIFY());
    
    RESPONSE PATCH(const URL& url, const BODY& body = BODY(), const HEADERS& headers = HEADERS(), 
                   const COOKIES& cookies = COOKIES(), const TIMEOUT& timeout = TIMEOUT(), 
                   const AUTH& auth = AUTH(), const PROXY& proxy = PROXY(), 
                   const REDIRECTS& redirects = REDIRECTS(), const VERIFY& verify = VERIFY());
    
    RESPONSE HEAD(const URL& url, const HEADERS& headers = HEADERS(), 
                  const COOKIES& cookies = COOKIES(), const TIMEOUT& timeout = TIMEOUT(), 
                  const AUTH& auth = AUTH(), const PROXY& proxy = PROXY(), 
                  const REDIRECTS& redirects = REDIRECTS(), const VERIFY& verify = VERIFY());
    
    RESPONSE OPTIONS(const URL& url, const HEADERS& headers = HEADERS(), 
                     const COOKIES& cookies = COOKIES(), const TIMEOUT& timeout = TIMEOUT(), 
                     const AUTH& auth = AUTH(), const PROXY& proxy = PROXY(), 
                     const REDIRECTS& redirects = REDIRECTS(), const VERIFY& verify = VERIFY());

    // Configuration methods with validation
    void set_default_headers(const HEADERS& headers);
    void set_default_cookies(const COOKIES& cookies);
    void set_cookie_jar(const std::string& file_path);
    
    // Performance tuning methods
    void set_connection_timeout(double seconds);
    void set_transfer_timeout(double seconds);
    void set_max_connections_per_host(size_t max_conns);
    void set_keep_alive(bool enable);
    void set_compression(bool enable);
    
    // Safety and monitoring methods
    bool is_valid() const noexcept;
    void reset() noexcept;
    size_t get_request_count() const noexcept;
    double get_average_response_time() const noexcept;
    
    // Connection pooling
    void enable_connection_pooling(bool enable = true);
    void set_pool_size(size_t size);

    // Get raw CURL handle with safety check
    CURL* get_curl_handle() const noexcept;

private:
    // Enhanced private members with safety features
    std::unique_ptr<CURL, std::function<void(CURL*)>> curl_handle_;
    HEADERS default_headers_;
    COOKIES default_cookies_;
    std::string cookie_jar_path_;
    
    // Performance monitoring
    std::atomic<size_t> request_count_{0};
    std::atomic<double> total_response_time_{0.0};
    mutable std::mutex stats_mutex_;
    
    // Connection pooling
    std::shared_ptr<SessionPool> connection_pool_;
    bool pooling_enabled_;
    
    // Safety mechanisms
    std::atomic<bool> is_valid_{false};
    mutable std::mutex session_mutex_;
    
    // Performance settings
    double connection_timeout_{30.0};
    double transfer_timeout_{60.0};
    size_t max_connections_per_host_{10};
    bool keep_alive_enabled_{true};
    bool compression_enabled_{true};
    
    // Private helper methods
    void initialize_curl_handle();
    void cleanup_curl_handle() noexcept;
    void validate_request(const REQUEST& request) const;
    void apply_performance_settings();
    void apply_safety_settings();
    void update_statistics(double response_time);
    
    // Thread-safe operations
    template<typename Func>
    auto with_lock(Func&& func) const -> decltype(func());
    
    // RAII wrapper for CURL resources
    class CurlHandleDeleter {
    public:
        void operator()(CURL* handle) const noexcept;
    };
};

// Thread-safe session pool for connection reuse
class SessionPool {
public:
    explicit SessionPool(size_t max_size = 100);
    ~SessionPool();
    
    std::shared_ptr<Session> acquire_session();
    void release_session(std::shared_ptr<Session> session);
    size_t size() const noexcept;
    size_t available() const noexcept;
    
private:
    mutable std::mutex pool_mutex_;
    std::condition_variable pool_cv_;
    std::vector<std::shared_ptr<Session>> available_sessions_;
    std::vector<std::shared_ptr<Session>> in_use_sessions_;
    size_t max_size_;
};

} // namespace CurlX
