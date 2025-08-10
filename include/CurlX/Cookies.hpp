#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <string_view>
#include <optional>
#include <initializer_list>

namespace CurlX {
    class COOKIES {
    public:
        COOKIES() = default;
        explicit COOKIES(std::unordered_map<std::string, std::string> cookie_map) : cookies_(std::move(cookie_map)) {}
        COOKIES(std::initializer_list<std::pair<std::string_view, std::string_view>> list) {
            for (const auto& [key, value] : list) {
                add(key, value);
            }
        }

        void add(std::string_view key, std::string_view value);
        void remove(std::string_view cookie_name);
        [[nodiscard]] std::optional<std::string> get(std::string_view cookie_name) const;
        [[nodiscard]] const std::unordered_map<std::string, std::string>& all() const noexcept;

    private:
        std::unordered_map<std::string, std::string> cookies_;
    };
}