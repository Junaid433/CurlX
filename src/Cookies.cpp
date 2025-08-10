#include "CurlX/Cookies.hpp"

namespace CurlX {

    void COOKIES::add(std::string_view key, std::string_view value) {
        cookies_[std::string(key)] = std::string(value);
    }

    void COOKIES::remove(std::string_view cookie_name) {
        cookies_.erase(std::string(cookie_name));
    }

    std::optional<std::string> COOKIES::get(std::string_view cookie_name) const {
        auto it = cookies_.find(std::string(cookie_name));
        if (it != cookies_.end()) {
            return it->second;
        }
        return std::nullopt;
    }

    const std::unordered_map<std::string, std::string>& COOKIES::all() const noexcept {
        return cookies_;
    }

} // namespace CurlX
