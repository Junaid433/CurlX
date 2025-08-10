#pragma once

#include "Url.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Session.hpp"

namespace CurlX {

    // Forward declaration of the helper function
    template<typename T>
    void apply_option(REQUEST& request, const T& option);

    // Variadic template PATCH function (uses a temporary session)
    template<typename... Args>
    RESPONSE PATCH(const URL& url, Args&&... args) {
        Session session; // Create a temporary session
        return PATCH(session, url, std::forward<Args>(args)...);
    }

    // Variadic template PATCH function (uses a provided session)
    template<typename... Args>
    CurlX::RESPONSE PATCH(Session& session, const URL& url, Args&&... args) {
        REQUEST request(url, METHOD::PATCH); // Start with URL and PATCH method

        // Apply each option to the request
        (apply_option(request, std::forward<Args>(args)), ...); // C++17 fold expression

        return session.send(request); // Send using the provided session
    }

} // namespace CurlX