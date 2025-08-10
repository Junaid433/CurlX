#pragma once
#include <string>
#include <string_view>

namespace CurlX {
    class URL {
    public:
        URL() = default;
        URL(std::string_view url) : url_str(url) {}
        URL(const char* url) : url_str(url) {}

        URL& operator=(std::string_view url) {
            url_str = url;
            return *this;
        }

        const std::string& toString() const { return url_str; }
        const char* c_str() const { return url_str.c_str(); }

    private:
        std::string url_str;
    };

    inline std::ostream& operator<<(std::ostream& os, const URL& url) {
        os << url.toString();
        return os;
    }
}
