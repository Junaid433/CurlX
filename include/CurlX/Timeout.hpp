#pragma once

namespace CurlX {
    struct TIMEOUT {
        long seconds;

        TIMEOUT(long s = 0L) : seconds(s) {}

        long value() const { return seconds; }
    };
}
