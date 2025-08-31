#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <optional>
#include "Headers.hpp"
#include "Url.hpp"
#include "Exceptions.hpp"
#include "Cookies.hpp"
#include <stdexcept>
#include <nlohmann/json.hpp>

namespace CurlX {

// Forward declaration of REQUEST to avoid circular dependency
struct REQUEST; 

class RESPONSE {
public:
    // Constructors with safety
    RESPONSE() = default;
    explicit RESPONSE(long status_code);
    RESPONSE(long status_code, std::string body);
    RESPONSE(long status_code, std::string body, HEADERS headers);
    
    // Copy and move operations
    RESPONSE(const RESPONSE& other);
    RESPONSE(RESPONSE&& other) noexcept;
    RESPONSE& operator=(const RESPONSE& other);
    RESPONSE& operator=(RESPONSE&& other) noexcept;
    
    // Destructor
    ~RESPONSE() = default;

    // Core response data with safety checks
    long statusCode{0};
    std::string reason; // Reason phrase for the status code
    URL url;            // Final URL after redirects
    bool is_redirect{false};   // True if the response was a redirect
    HEADERS headers;
    std::string body;
    URL request_url; // The URL that was requested
    HEADERS request_headers; // The headers that were sent with the request
    COOKIES received_cookies; // Cookies received in the response
    double elapsed_time{0.0};      // Time taken for the request in seconds
    std::vector<URL> history; // Redirect history
    
    // Additional safety and performance fields
    std::chrono::steady_clock::time_point timestamp;
    size_t content_length{0};
    std::string content_type;
    std::string encoding;
    bool is_compressed{false};
    std::string server_info;
    std::string last_modified;
    std::string etag;
    
    // Safety validation methods
    bool is_valid() const noexcept;
    void validate() const;
    
    // Status checking with enhanced safety
    bool ok() const noexcept;
    bool is_client_error() const noexcept;
    bool is_server_error() const noexcept;
    bool is_redirect_status() const noexcept;
    bool is_informational() const noexcept;
    bool is_success() const noexcept;
    
    // Enhanced error handling
    void raise_for_status() const;
    void raise_for_status_verbose() const;
    
    // Safe content access methods
    std::string text() const noexcept;
    std::string_view text_view() const noexcept;
    
    // Enhanced JSON parsing with safety
    nlohmann::json json() const;
    std::optional<nlohmann::json> json_safe() const noexcept;
    
    // Performance and monitoring methods
    double get_response_time() const noexcept;
    size_t get_content_size() const noexcept;
    std::string get_content_type() const noexcept;
    bool is_json() const noexcept;
    bool is_xml() const noexcept;
    bool is_html() const noexcept;
    bool is_text() const noexcept;
    
    // Header utilities with safety
    std::optional<std::string> get_header(std::string_view name) const noexcept;
    bool has_header(std::string_view name) const noexcept;
    std::vector<std::string> get_headers(std::string_view name) const noexcept;
    
    // Cookie utilities
    const COOKIES& get_cookies() const noexcept;
    std::optional<std::string> get_cookie(std::string_view name) const noexcept;
    
    // URL and redirect utilities
    const URL& get_final_url() const noexcept;
    const std::vector<URL>& get_redirect_history() const noexcept;
    size_t get_redirect_count() const noexcept;
    
    // Content analysis
    bool is_empty() const noexcept;
    bool is_binary() const noexcept;
    size_t estimate_memory_usage() const noexcept;
    
    // Safety checks for large responses
    bool is_response_too_large() const noexcept;
    void truncate_if_needed(size_t max_size);
    
    // Performance optimization methods
    void optimize_for_reading();
    void compress_if_beneficial();
    
    // Debugging and diagnostics
    std::string get_debug_info() const;
    void log_performance_metrics() const;
    
    // Memory management
    void clear_body();
    void reserve_body_capacity(size_t capacity);
    void shrink_body_to_fit();

private:
    // Internal validation helpers
    bool validate_status_code(long code) const noexcept;
    bool validate_content_length(size_t length) const noexcept;
    bool validate_headers() const noexcept;
    
    // Content type detection
    void detect_content_type();
    bool is_binary_content() const noexcept;
    
    // Performance optimization helpers
    void optimize_headers();
    void optimize_cookies();
    
    // Safety constants
    static constexpr size_t MAX_BODY_SIZE = 100 * 1024 * 1024; // 100MB
    static constexpr size_t MAX_HEADERS_SIZE = 64 * 1024; // 64KB
    static constexpr size_t MAX_URL_LENGTH = 2048; // 2KB
    static constexpr double MAX_RESPONSE_TIME = 3600.0; // 1 hour
    
    // Internal state
    mutable std::optional<nlohmann::json> cached_json_;
    mutable bool content_type_detected_{false};
    mutable bool optimized_{false};
};

// Utility functions for response handling
namespace ResponseUtils {
    // Safe response creation
    RESPONSE create_success_response(long status_code, std::string body = "");
    RESPONSE create_error_response(long status_code, std::string reason = "");
    RESPONSE create_redirect_response(long status_code, const URL& location);
    
    // Response validation
    bool is_valid_status_code(long code) noexcept;
    bool is_safe_response_size(size_t size) noexcept;
    
    // Performance helpers
    double calculate_response_efficiency(const RESPONSE& response);
    size_t estimate_memory_footprint(const RESPONSE& response);
    
    // Content analysis
    std::string detect_mime_type(const std::string& content);
    bool is_compressed_content(const std::string& content_encoding);
}

} // namespace CurlX