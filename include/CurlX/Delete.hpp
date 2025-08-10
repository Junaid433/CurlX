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
#include "Method.hpp" // Include Method.hpp for METHOD::DELETE

namespace CurlX {

    // Forward declaration of the helper function
    template<typename T>
    void apply_option(REQUEST& request, const T& option);

    // Variadic template DELETE function (uses a temporary session)
    template<typename... Args>
    CurlX::RESPONSE DELETE(const URL& url, Args&&... args) {
        Session session; // Create a temporary session
        return DELETE(session, url, std::forward<Args>(args)...);
    }

    // Variadic template DELETE function (uses a provided session)
    template<typename... Args>
    CurlX::RESPONSE DELETE(Session& session, const URL& url, Args&&... args) {
        REQUEST request(url, METHOD::DELETE); // Start with URL and DELETE method

        // Apply each option to the request
        (apply_option(request, std::forward<Args>(args)), ...); // C++17 fold expression

        return session.send(request); // Send using the provided session
    }

} // namespace CurlX