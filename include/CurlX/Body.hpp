#pragma once
#include <string>
#include <string_view>

namespace CurlX {
    class BODY {
    public:
        BODY() = default;
        BODY(std::string_view body) : body_str(body) {}
        BODY(const char* body) : body_str(body) {}

        BODY& operator=(std::string_view body) {
            body_str = body;
            return *this;
        }

        const std::string& toString() const { return body_str; }
        const char* c_str() const { return body_str.c_str(); }
        size_t length() const { return body_str.length(); }

    private:
        std::string body_str;
    };
}
