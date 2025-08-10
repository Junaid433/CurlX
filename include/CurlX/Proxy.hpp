#pragma once

#include <string>

namespace CurlX {
    struct PROXY {
        std::string value;

        PROXY(std::string v = "") : value(std::move(v)) {}

        std::string str() const { return value; }
    };
}
