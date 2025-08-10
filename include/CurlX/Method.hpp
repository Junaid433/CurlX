#pragma once

#include <string>

namespace CurlX {
    struct METHOD {
        std::string value;

        METHOD(std::string v = "GET") : value(std::move(v)) {}
        METHOD(const char* v) : value(v) {}

        METHOD& operator=(std::string_view v) {
            value = v;
            return *this;
        }

        const char* c_str() const { return value.c_str(); }

        // Static members for common HTTP methods
        static const METHOD GET;
        static const METHOD POST;
        static const METHOD PUT;
        static const METHOD DELETE;
        static const METHOD HEAD;
        static const METHOD OPTIONS;
        static const METHOD PATCH;
    };
}
