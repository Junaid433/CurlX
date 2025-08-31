#pragma once

#include <stdexcept>
#include <string>

namespace CurlX {

// Basic exception classes for now - can be expanded later
class RequestException : public std::runtime_error {
public:
    explicit RequestException(const std::string& message) : std::runtime_error(message) {}
};

class ConnectionError : public RequestException {
public:
    explicit ConnectionError(const std::string& message) : RequestException("Connection Error: " + message) {}
};

class Timeout : public RequestException {
public:
    explicit Timeout(const std::string& message) : RequestException("Timeout: " + message) {}
};

class HTTPError : public RequestException {
public:
    explicit HTTPError(const std::string& message) : RequestException("HTTP Error: " + message) {}
};

class TooManyRedirects : public RequestException {
public:
    explicit TooManyRedirects(const std::string& message) : RequestException("Too Many Redirects: " + message) {}
};

} // namespace CurlX
