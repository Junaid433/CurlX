#pragma once

#include "Url.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Session.hpp"

namespace CurlX {

    // Forward declaration of the helper function
    template<typename T>
    void apply_option(REQUEST& request, const T& option);

    // Variadic template HEAD function (uses a temporary session)
    template<typename... Args>
    RESPONSE HEAD(const URL& url, Args&&... args) {
        Session session; // Create a temporary session
        return HEAD(session, url, std::forward<Args>(args)...);
    }

    // Variadic template HEAD function (uses a provided session)
    template<typename... Args>
    CurlX::RESPONSE HEAD(Session& session, const URL& url, Args&&... args) {
        REQUEST request(url, METHOD::HEAD); // Start with URL and HEAD method

        // Apply each option to the request
        (apply_option(request, std::forward<Args>(args)), ...); // C++17 fold expression

        return session.send(request); // Send using the provided session
    }

} // namespace CurlX
