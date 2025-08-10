#pragma once
#include <vector>
#include <string>
#include <string_view>
#include <optional>
#include <unordered_map>
#include <initializer_list>

struct curl_slist;

namespace CurlX {
    class HEADERS {
        public:
            HEADERS() = default;
            explicit HEADERS(std::vector<std::string> hdrs) : headers_(std::move(hdrs)) {}
            HEADERS(const std::unordered_map<std::string, std::string>& headers_map) {
                for (const auto& [key, value] : headers_map) {
                    add(key, value);
                }
            }
            HEADERS(std::initializer_list<std::pair<std::string_view, std::string_view>> list) {
                for (const auto& [key, value] : list) {
                    add(key, value);
                }
            }
            void add(std::string_view key, std::string_view value);
            void add(std::string_view header_line);
            void remove(std::string_view header_name);
            [[nodiscard]] std::optional<std::string> get(std::string_view header_name) const;
            [[nodiscard]] const std::vector<std::string>& all() const noexcept;
            struct curl_slist* to_curl_slist() const;
            static void free_curl_slist(struct curl_slist* list);

            // Iterators for range-based for loop
            auto begin() const { return headers_.begin(); }
            auto end() const { return headers_.end(); }

        private:
            std::vector<std::string> headers_;
    };
}
