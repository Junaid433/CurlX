#include "CurlX/Headers.hpp"
#include <curl/curl.h>
#include <algorithm>
#include <string>
#include <stdexcept>
#include <cassert>
#include <cstring>

namespace CurlX {

// Enhanced header validation and safety
namespace {
    constexpr size_t MAX_HEADER_SIZE = 8192;  // 8KB limit per header
    constexpr size_t MAX_HEADERS_COUNT = 1000; // Maximum number of headers
    constexpr size_t MAX_HEADER_NAME_SIZE = 256; // Maximum header name length
    constexpr size_t MAX_HEADER_VALUE_SIZE = 4096; // Maximum header value length
    
    // Safe string operations with bounds checking
    bool is_valid_header_name(std::string_view name) noexcept {
        if (name.empty() || name.length() > MAX_HEADER_NAME_SIZE) {
            return false;
        }
        
        // Check for valid characters in header names
        for (char c : name) {
            if (c < 32 || c > 126 || c == ':') {
                return false;
            }
        }
        
        return true;
    }
    
    bool is_valid_header_value(std::string_view value) noexcept {
        if (value.length() > MAX_HEADER_VALUE_SIZE) {
            return false;
        }
        
        // Check for valid characters in header values
        for (char c : value) {
            if (c < 32 && c != '\t') {
                return false;
            }
        }
        
        return true;
    }
    
    // Safe string concatenation with bounds checking
    std::string safe_concat(std::string_view a, std::string_view b, std::string_view separator = ": ") {
        if (a.length() + b.length() + separator.length() > MAX_HEADER_SIZE) {
            throw std::length_error("Header size exceeds maximum allowed size");
        }
        return std::string(a) + std::string(separator) + std::string(b);
    }
    
    // Safe string search with bounds checking
    size_t safe_find(const std::string& str, const std::string& search, size_t pos = 0) noexcept {
        if (pos >= str.length()) return std::string::npos;
        if (search.empty()) return pos;
        if (search.length() > str.length() - pos) return std::string::npos;
        
        return str.find(search, pos);
    }
    
    // Safe substring with bounds checking
    std::string safe_substr(const std::string& str, size_t pos, size_t len = std::string::npos) noexcept {
        if (pos >= str.length()) return {};
        if (len == std::string::npos || pos + len > str.length()) {
            len = str.length() - pos;
        }
        return str.substr(pos, len);
    }
}

// Constructors
HEADERS::HEADERS(size_t initial_capacity) {
    if (initial_capacity > MAX_HEADERS_COUNT) {
        initial_capacity = MAX_HEADERS_COUNT;
    }
    headers_.reserve(initial_capacity);
}

HEADERS::HEADERS(const HEADERS& other) : headers_(other.headers_) {}

HEADERS::HEADERS(HEADERS&& other) noexcept : headers_(std::move(other.headers_)) {}

HEADERS& HEADERS::operator=(const HEADERS& other) {
    if (this != &other) {
        headers_ = other.headers_;
    }
    return *this;
}

HEADERS& HEADERS::operator=(HEADERS&& other) noexcept {
    if (this != &other) {
        headers_ = std::move(other.headers_);
    }
    return *this;
}

void HEADERS::add(std::string_view key, std::string_view value) {
    // Validate input parameters
    if (!is_valid_header_name(key)) {
        throw std::invalid_argument("Invalid header name: " + std::string(key));
    }
    
    if (!is_valid_header_value(value)) {
        throw std::invalid_argument("Invalid header value: " + std::string(value));
    }
    
    // Check if we're at the limit
    if (headers_.size() >= MAX_HEADERS_COUNT) {
        throw std::length_error("Maximum number of headers exceeded");
    }
    
    try {
        std::string header = safe_concat(key, value);
        headers_.push_back(std::move(header));
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to add header: " + std::string(e.what()));
    }
}

void HEADERS::add(std::string_view header_line) {
    // Validate header line
    if (header_line.empty() || header_line.length() > MAX_HEADER_SIZE) {
        throw std::invalid_argument("Invalid header line: " + std::string(header_line));
    }
    
    // Check if we're at the limit
    if (headers_.size() >= MAX_HEADERS_COUNT) {
        throw std::length_error("Maximum number of headers exceeded");
    }
    
    // Validate header format (must contain ':')
    size_t colon_pos = header_line.find(':');
    if (colon_pos == std::string::npos || colon_pos == 0 || colon_pos == header_line.length() - 1) {
        throw std::invalid_argument("Invalid header format: " + std::string(header_line));
    }
    
    // Extract and validate name and value
    std::string_view name = header_line.substr(0, colon_pos);
    std::string_view value = header_line.substr(colon_pos + 1);
    
    if (!is_valid_header_name(name)) {
        throw std::invalid_argument("Invalid header name in line: " + std::string(header_line));
    }
    
    if (!is_valid_header_value(value)) {
        throw std::invalid_argument("Invalid header value in line: " + std::string(header_line));
    }
    
    try {
        headers_.emplace_back(header_line);
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to add header line: " + std::string(e.what()));
    }
}

void HEADERS::remove(std::string_view header_name) {
    if (header_name.empty() || !is_valid_header_name(header_name)) {
        return; // Silently ignore invalid header names
    }
    
    try {
        headers_.erase(
            std::remove_if(headers_.begin(), headers_.end(),
                [&header_name](const std::string& header) {
                    // Case-insensitive comparison with bounds checking
                    if (header.length() < header_name.length()) return false;
                    
                    std::string header_lower = header;
                    std::string name_lower = std::string(header_name);
                    
                    // Convert to lowercase for comparison
                    std::transform(header_lower.begin(), header_lower.end(), header_lower.begin(), ::tolower);
                    std::transform(name_lower.begin(), name_lower.end(), name_lower.begin(), ::tolower);
                    
                    return header_lower.rfind(name_lower, 0) == 0;
                }),
            headers_.end()
        );
    } catch (const std::exception&) {
        // Silently ignore errors during removal
    }
}

std::optional<std::string> HEADERS::get(std::string_view header_name) const {
    if (header_name.empty() || !is_valid_header_name(header_name)) {
        return std::nullopt;
    }
    
    try {
        for (const auto& header : headers_) {
            if (header.length() < header_name.length()) continue;
            
            // Case-insensitive comparison
            std::string header_lower = header;
            std::string name_lower = std::string(header_name);
            
            std::transform(header_lower.begin(), header_lower.end(), header_lower.begin(), ::tolower);
            std::transform(name_lower.begin(), name_lower.end(), name_lower.begin(), ::tolower);
            
            if (header_lower.rfind(name_lower, 0) == 0) {
                size_t colon_pos = safe_find(header, ":");
                if (colon_pos != std::string::npos && colon_pos + 2 < header.length()) {
                    return safe_substr(header, colon_pos + 2);
                }
            }
        }
    } catch (const std::exception&) {
        // Return nullopt on any error
    }
    
    return std::nullopt;
}

const std::vector<std::string>& HEADERS::all() const noexcept {
    return headers_;
}

struct curl_slist* HEADERS::to_curl_slist() const {
    struct curl_slist* list = nullptr;
    
    try {
        for (const auto& header : headers_) {
            if (!header.empty() && header.length() <= MAX_HEADER_SIZE) {
                struct curl_slist* new_item = curl_slist_append(list, header.c_str());
                if (!new_item) {
                    // Clean up on failure
                    if (list) curl_slist_free_all(list);
                    return nullptr;
                }
                list = new_item;
            }
        }
    } catch (const std::exception&) {
        // Clean up on any error
        if (list) curl_slist_free_all(list);
        return nullptr;
    }
    
    return list;
}

void HEADERS::free_curl_slist(struct curl_slist* list) noexcept {
    if (list) {
        curl_slist_free_all(list);
    }
}

// Additional safety methods
void HEADERS::clear() noexcept {
    try {
        headers_.clear();
    } catch (const std::exception&) {
        // Silently ignore errors during clear
    }
}

size_t HEADERS::size() const noexcept {
    return headers_.size();
}

bool HEADERS::empty() const noexcept {
    return headers_.empty();
}

void HEADERS::reserve(size_t capacity) {
    if (capacity > MAX_HEADERS_COUNT) {
        capacity = MAX_HEADERS_COUNT;
    }
    
    try {
        headers_.reserve(capacity);
    } catch (const std::exception&) {
        // Silently ignore allocation errors
    }
}

// Safe iteration with bounds checking
std::vector<std::string>::const_iterator HEADERS::begin() const noexcept {
    return headers_.begin();
}

std::vector<std::string>::const_iterator HEADERS::end() const noexcept {
    return headers_.end();
}

// Validation method
bool HEADERS::is_valid() const noexcept {
    try {
        for (const auto& header : headers_) {
            if (header.empty() || header.length() > MAX_HEADER_SIZE) {
                return false;
            }
            
            size_t colon_pos = header.find(':');
            if (colon_pos == std::string::npos || colon_pos == 0 || colon_pos == header.length() - 1) {
                return false;
            }
            
            std::string_view name = header.substr(0, colon_pos);
            std::string_view value = header.substr(colon_pos + 1);
            
            if (!is_valid_header_name(name) || !is_valid_header_value(value)) {
                return false;
            }
        }
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

// Performance optimization: bulk operations
void HEADERS::add_bulk(const std::vector<std::pair<std::string, std::string>>& header_pairs) {
    if (headers_.size() + header_pairs.size() > MAX_HEADERS_COUNT) {
        throw std::length_error("Adding these headers would exceed maximum count");
    }
    
    try {
        headers_.reserve(headers_.size() + header_pairs.size());
        
        for (const auto& [key, value] : header_pairs) {
            add(key, value);
        }
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to add bulk headers: " + std::string(e.what()));
    }
}

// Case-insensitive search
bool HEADERS::has(std::string_view header_name) const noexcept {
    if (header_name.empty() || !is_valid_header_name(header_name)) {
        return false;
    }
    
    try {
        std::string name_lower = std::string(header_name);
        std::transform(name_lower.begin(), name_lower.end(), name_lower.begin(), ::tolower);
        
        for (const auto& header : headers_) {
            if (header.length() < header_name.length()) continue;
            
            std::string header_lower = header;
            std::transform(header_lower.begin(), header_lower.end(), header_lower.begin(), ::tolower);
            
            if (header_lower.rfind(name_lower, 0) == 0) {
                return true;
            }
        }
    } catch (const std::exception&) {
        // Return false on any error
    }
    
    return false;
}

// Case-insensitive operations
std::optional<std::string> HEADERS::get_case_insensitive(std::string_view header_name) const noexcept {
    return get(header_name); // Already case-insensitive
}

void HEADERS::remove_case_insensitive(std::string_view header_name) noexcept {
    remove(header_name); // Already case-insensitive
}

} // namespace CurlX
