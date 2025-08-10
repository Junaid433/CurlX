#pragma once

#include <string>
#include <vector>
#include "Headers.hpp"
#include "Url.hpp"
#include "Exceptions.hpp" // Include Exceptions.hpp
#include "Cookies.hpp" // Include Cookies.hpp for received_cookies
#include <stdexcept>
#include <nlohmann/json.hpp>

namespace CurlX {

    // Forward declaration of REQUEST to avoid circular dependency if needed
    struct REQUEST; 

    struct RESPONSE {
        long statusCode;
        std::string reason; // Reason phrase for the status code
        URL url;            // Final URL after redirects
        bool is_redirect;   // True if the response was a redirect
        HEADERS headers;
        std::string body;
        URL request_url; // The URL that was requested
        HEADERS request_headers; // The headers that were sent with the request
        COOKIES received_cookies; // Cookies received in the response
        double elapsed_time;      // Time taken for the request in seconds
        std::vector<URL> history; // Redirect history

        // Check if the request was successful (status code < 400)
        bool ok() const {
            return statusCode >= 200 && statusCode < 400;
        }

        // Raise an exception for bad status codes (4xx or 5xx)
        void raise_for_status() const {
            if (!ok()) {
                throw HTTPError("HTTP Error: " + std::to_string(statusCode));
            }
        }

        // Return the body as a string (already available)
        std::string text() const {
            return body;
        }

        // Return the body as a JSON object
        nlohmann::json json() const {
            try {
                return nlohmann::json::parse(body);
            } catch (const nlohmann::json::parse_error& e) {
                throw RequestException("JSON parse error: " + std::string(e.what()));
            }
        }
    };
}