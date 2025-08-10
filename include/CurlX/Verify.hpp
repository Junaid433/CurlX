#pragma once

namespace CurlX {
    struct VERIFY {
        bool value;

        VERIFY(bool v = true) : value(v) {}

        bool perform() const { return value; }
    };
}
