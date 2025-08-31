#pragma once

#include <string>
#include <vector>
#include <optional>
#include <string_view>
#include <curl/curl.h>

namespace CurlX {

class HEADERS {
public:
    // Constructors
    HEADERS() = default;
    explicit HEADERS(size_t initial_capacity);
    
    // Copy and move operations
    HEADERS(const HEADERS& other);
    HEADERS(HEADERS&& other) noexcept;
    HEADERS& operator=(const HEADERS& other);
    HEADERS& operator=(HEADERS&& other) noexcept;
    
    // Destructor
    ~HEADERS() = default;

    // Core methods with enhanced safety
    void add(std::string_view key, std::string_view value);
    void add(std::string_view header_line);
    void remove(std::string_view header_name);
    std::optional<std::string> get(std::string_view header_name) const;
    
    // Enhanced access methods
    const std::vector<std::string>& all() const noexcept;
    
    // CURL integration with safety
    struct curl_slist* to_curl_slist() const;
    void free_curl_slist(struct curl_slist* list) noexcept;
    
    // Additional safety methods
    void clear() noexcept;
    size_t size() const noexcept;
    bool empty() const noexcept;
    void reserve(size_t capacity);
    
    // Safe iteration
    std::vector<std::string>::const_iterator begin() const noexcept;
    std::vector<std::string>::const_iterator end() const noexcept;
    
    // Validation
    bool is_valid() const noexcept;
    
    // Performance optimizations
    void add_bulk(const std::vector<std::pair<std::string, std::string>>& header_pairs);
    bool has(std::string_view header_name) const noexcept;
    
    // Case-insensitive operations
    std::optional<std::string> get_case_insensitive(std::string_view header_name) const noexcept;
    void remove_case_insensitive(std::string_view header_name) noexcept;
    
    // Bulk operations for performance
    template<typename Container>
    void add_from_container(const Container& container) {
        static_assert(std::is_same_v<typename Container::value_type, std::pair<std::string, std::string>>,
                     "Container must contain std::pair<std::string, std::string>");
        add_bulk(std::vector<std::pair<std::string, std::string>>(container.begin(), container.end()));
    }

private:
    std::vector<std::string> headers_;
    
    // Internal validation helpers
    bool validate_header_name(std::string_view name) const noexcept;
    bool validate_header_value(std::string_view value) const noexcept;
    bool validate_header_line(std::string_view line) const noexcept;
    
    // Memory management
    void ensure_capacity(size_t additional_size);
    
    // Constants for safety limits
    static constexpr size_t MAX_HEADER_SIZE = 8192;
    static constexpr size_t MAX_HEADERS_COUNT = 1000;
    static constexpr size_t MAX_HEADER_NAME_SIZE = 256;
    static constexpr size_t MAX_HEADER_VALUE_SIZE = 4096;
};

} // namespace CurlX
