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
#include "Method.hpp" // Include Method.hpp for METHOD::PUT

namespace CurlX {

    // Forward declaration of the helper function
    template<typename T>
    void apply_option(REQUEST& request, const T& option);

    // Variadic template PUT function (uses a temporary session)
    template<typename... Args>
    RESPONSE PUT(const URL& url, Args&&... args) {
        Session session; // Create a temporary session
        return PUT(session, url, std::forward<Args>(args)...);
    }

    // Variadic template PUT function (uses a provided session)
    template<typename... Args>
    CurlX::RESPONSE PUT(Session& session, const URL& url, Args&&... args) {
        REQUEST request(url, METHOD::PUT); // Start with URL and PUT method

        // Apply each option to the request
        (apply_option(request, std::forward<Args>(args)), ...); // C++17 fold expression

        return session.send(request); // Send using the provided session
    }

} // namespace CurlX