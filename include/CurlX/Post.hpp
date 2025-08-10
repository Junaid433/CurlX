#pragma once

#include "Response.hpp"
#include "Url.hpp"
#include "Headers.hpp"
#include "Body.hpp"
#include "Timeout.hpp"
#include "Auth.hpp"
#include "Proxy.hpp"
#include "Cookies.hpp"
#include "Redirects.hpp"
#include "Verify.hpp"
#include "Request.hpp"
#include "Session.hpp"

namespace CurlX {

    // Forward declaration of the helper function
    template<typename T>
    void apply_option(REQUEST& request, const T& option);

    // Variadic template POST function (uses a temporary session)
    template<typename... Args>
    RESPONSE POST(const URL& url, Args&&... args) {
        Session session; // Create a temporary session
        return POST(session, url, std::forward<Args>(args)...);
    }

    // Variadic template POST function (uses a provided session)
    template<typename... Args>
    CurlX::RESPONSE POST(Session& session, const URL& url, Args&&... args) {
        REQUEST request(url, METHOD::POST); // Start with URL and POST method

        // Apply each option to the request
        (apply_option(request, std::forward<Args>(args)), ...); // C++17 fold expression

        return session.send(request); // Send using the provided session
    }

} // namespace CurlX