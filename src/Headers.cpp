#include "CurlX/Headers.hpp"
#include <curl/curl.h>
#include <algorithm>
#include <string>

namespace CurlX {

void HEADERS::add(std::string_view key, std::string_view value) {
    std::string header = std::string(key) + ": " + std::string(value);
    headers_.push_back(header);
}

void HEADERS::add(std::string_view header_line) {
    headers_.emplace_back(header_line);
}

void HEADERS::remove(std::string_view header_name) {
    headers_.erase(std::remove_if(headers_.begin(), headers_.end(),
                                  [&](const std::string& header) {
                                      return header.rfind(header_name, 0) == 0;
                                  }),
                   headers_.end());
}

std::optional<std::string> HEADERS::get(std::string_view header_name) const {
    for (const auto& header : headers_) {
        if (header.rfind(header_name, 0) == 0) {
            size_t colon_pos = header.find(':');
            if (colon_pos != std::string::npos) {
                return header.substr(colon_pos + 2);
            }
        }
    }
    return std::nullopt;
}

const std::vector<std::string>& HEADERS::all() const noexcept {
    return headers_;
}

struct curl_slist* HEADERS::to_curl_slist() const {
    struct curl_slist* list = nullptr;
    for (const auto& header : headers_) {
        list = curl_slist_append(list, header.c_str());
    }
    return list;
}

void HEADERS::free_curl_slist(struct curl_slist* list) {
    if (list) {
        curl_slist_free_all(list);
    }
}

} // namespace CurlX
