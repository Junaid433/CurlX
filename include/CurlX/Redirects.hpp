#pragma once

namespace CurlX {
    struct REDIRECTS {
        bool value;
        long max_redirects; // Use long for CURLOPT_MAXREDIRS

        // Default constructor: allow redirects, max 30
        REDIRECTS() : value(true), max_redirects(30L) {}

        // Constructor to explicitly set allow/disallow
        explicit REDIRECTS(bool v) : value(v), max_redirects(v ? 30L : 0L) {}

        // Constructor to set allow and max_redirects
        REDIRECTS(bool v, long max_r) : value(v), max_redirects(max_r) {}

        bool allow() const { return value; }
        long getMaxRedirects() const { return max_redirects; }
    };
}
