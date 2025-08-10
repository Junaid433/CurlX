# CurlX API Reference

This document provides a detailed reference for the CurlX library's public API.

## Core Components

### `CurlX::Session`

The `Session` class manages the underlying `libcurl` handle and provides methods for sending HTTP requests. It allows for persistent connections, cookie management, and setting default request options.

**Key Methods:**

*   **`Session()`**: Constructor.
*   **`~Session()`**: Destructor.
*   **`RESPONSE send(const REQUEST& request)`**: Sends a pre-configured `REQUEST` object.
*   **`RESPONSE GET(const URL& url, ...)`**: Sends a GET request.
*   **`RESPONSE POST(const URL& url, ...)`**: Sends a POST request.
*   **`RESPONSE PUT(const URL& url, ...)`**: Sends a PUT request.
*   **`RESPONSE DELETE(const URL& url, ...)`**: Sends a DELETE request.
*   **`RESPONSE PATCH(const URL& url, ...)`**: Sends a PATCH request.
*   **`RESPONSE HEAD(const URL& url, ...)`**: Sends a HEAD request.
*   **`RESPONSE OPTIONS(const URL& url, ...)`**: Sends an OPTIONS request.
*   **`void set_default_headers(const HEADERS& headers)`**: Sets default headers for all subsequent requests in this session.
*   **`void set_default_cookies(const COOKIES& cookies)`**: Sets default cookies for all subsequent requests in this session.
*   **`void set_cookie_jar(const std::string& file_path)`**: Configures a cookie jar file for persistent cookie storage.
*   **`CURL* get_curl_handle()`**: Returns the underlying `CURL` handle (for advanced use).

### `CurlX::REQUEST`

The `REQUEST` struct encapsulates all the details of an HTTP request. It is designed to be built using chainable setters.

**Key Members & Chainable Setters:**

*   **`REQUEST& url(const URL& u)`**: Sets the target URL.
*   **`REQUEST& method(const METHOD& m)`**: Sets the HTTP method (e.g., `METHOD::GET`, `METHOD::POST`).
*   **`REQUEST& headers(const HEADERS& h)`**: Sets request headers.
*   **`REQUEST& params(const PARAMS& p)`**: Sets URL query parameters.
*   **`REQUEST& cookies(const COOKIES& c)`**: Sets cookies to be sent with the request.
*   **`REQUEST& body(const BODY& b)`**: Sets the request body.
*   **`REQUEST& timeout(const TIMEOUT& t)`**: Sets the request timeout.
*   **`REQUEST& auth(const AUTH& a)`**: Sets authentication credentials.
*   **`REQUEST& proxy(const PROXY& p)`**: Sets proxy settings.
*   **`REQUEST& redirects(const REDIRECTS& r)`**: Configures redirect handling.
*   **`REQUEST& verify(const VERIFY& v)`**: Configures SSL certificate verification.
*   **`REQUEST& files(const FILES& f)`**: Sets files for multipart form data uploads.
*   **`REQUEST& output_file_path(const std::string& ofp)`**: Specifies a file path to write the response body to.
*   **`REQUEST& write_callback(WriteCallback cb, void* userdata = nullptr)`**: Sets a custom write callback for response data.
*   **`REQUEST& read_callback(ReadCallback cb, void* userdata = nullptr)`**: Sets a custom read callback for request body data.

### `CurlX::RESPONSE`

The `RESPONSE` struct holds all the information returned from an HTTP request.

**Key Members:**

*   **`long statusCode`**: The HTTP status code (e.g., 200, 404).
*   **`std::string reason`**: The reason phrase associated with the status code.
*   **`URL url`**: The final URL after any redirects.
*   **`bool is_redirect`**: True if the response was a redirect.
*   **`HEADERS headers`**: Response headers.
*   **`std::string body`**: The response body.
*   **`URL request_url`**: The URL that was originally requested.
*   **`HEADERS request_headers`**: The headers sent with the request.
*   **`COOKIES received_cookies`**: Cookies received in the response.
*   **`double elapsed_time`**: Time taken for the request in seconds.
*   **`std::vector<URL> history`**: A history of URLs if redirects occurred.

**Utility Methods:**

*   **`bool ok() const`**: Returns `true` if `statusCode` is in the 2xx or 3xx range.
*   **`void raise_for_status() const`**: Throws an `HTTPError` exception if `statusCode` is 4xx or 5xx.
*   **`std::string text() const`**: Returns the response body as a string.
*   **`nlohmann::json json() const`**: Parses and returns the response body as a `nlohmann::json` object. Throws `RequestException` on parse error.

## Data Types & Options

### `CurlX::URL`

A wrapper around `std::string` for representing URLs.

**Key Methods:**

*   **`URL(std::string_view url)`**: Constructor.
*   **`const std::string& toString() const`**: Returns the URL as a string.
*   **`const char* c_str() const`**: Returns the URL as a C-style string.

### `CurlX::HEADERS`

Manages HTTP request and response headers.

**Key Methods:**

*   **`void add(std::string_view key, std::string_view value)`**: Adds a header.
*   **`void add(std::string_view header_line)`**: Adds a header from a full line (e.g., "Content-Type: application/json").
*   **`void remove(std::string_view header_name)`**: Removes headers by name.
*   **`std::optional<std::string> get(std::string_view header_name) const`**: Retrieves a header value.
*   **`const std::vector<std::string>& all() const noexcept`**: Returns all headers as a vector of strings.

### `CurlX::BODY`

Represents the request body.

**Key Methods:**

*   **`BODY(std::string_view body)`**: Constructor.
*   **`const std::string& toString() const`**: Returns the body as a string.

### `CurlX::COOKIES`

Manages HTTP cookies.

**Key Methods:**

*   **`void add(std::string_view key, std::string_view value)`**: Adds a cookie.
*   **`void remove(std::string_view cookie_name)`**: Removes a cookie by name.
*   **`std::optional<std::string> get(std::string_view cookie_name) const`**: Retrieves a cookie value.
*   **`const std::unordered_map<std::string, std::string>& all() const noexcept`**: Returns all cookies as a map.

### `CurlX::AUTH`

Handles authentication credentials.

**Key Methods:**

*   **`AUTH(std::string username, std::string password, AuthType type = AuthType::Basic)`**: Constructor for username/password.
*   **`explicit AUTH(std::string user_pass_string)`**: Constructor for "user:pass" string.
*   **`AuthType type() const`**: Returns the authentication type.
*   **`const std::string& username() const`**: Returns the username.
*   **`const std::string& password() const`**: Returns the password.

### `CurlX::PROXY`

Configures proxy settings.

**Key Methods:**

*   **`PROXY(std::string v = "")`**: Constructor, takes proxy string (e.g., "http://host:port").
*   **`std::string str() const`**: Returns the proxy string.

### `CurlX::TIMEOUT`

Sets the request timeout.

**Key Methods:**

*   **`TIMEOUT(long seconds = 0L)`**: Constructor, takes timeout in seconds.
*   **`long value() const`**: Returns the timeout value.

### `CurlX::REDIRECTS`

Controls HTTP redirect behavior.

**Key Methods:**

*   **`REDIRECTS()`**: Default constructor (allows redirects, max 30).
*   **`explicit REDIRECTS(bool v)`**: Constructor to enable/disable redirects.
*   **`REDIRECTS(bool v, long max_r)`**: Constructor to enable/disable and set max redirects.
*   **`bool allow() const`**: Returns true if redirects are allowed.
*   **`long getMaxRedirects() const`**: Returns the maximum number of redirects.

### `CurlX::VERIFY`

Controls SSL certificate verification.

**Key Methods:**

*   **`VERIFY(bool v = true)`**: Constructor, `true` for verification, `false` to disable.
*   **`bool perform() const`**: Returns the verification setting.

### `CurlX::METHOD`

Represents an HTTP method.

**Key Members:**

*   **`std::string value`**: The method string (e.g., "GET", "POST").
*   **Static Constants**: `METHOD::GET`, `METHOD::POST`, `METHOD::PUT`, `METHOD::DELETE`, `METHOD::PATCH`, `METHOD::HEAD`, `METHOD::OPTIONS`.

### `CurlX::PARAMS`

Manages URL query parameters.

**Key Methods:**

*   **`PARAMS(StringMap p)`**: Constructor from a `std::map<std::string, std::string>`.
*   **`const std::map<std::string, std::string>& get() const`**: Returns the parameters map.

### `CurlX::FILES`

Manages files for multipart form data uploads.

**Key Methods:**

*   **`FILES(std::vector<std::pair<std::string, std::string>> f)`**: Constructor from a vector of field name/file path pairs.
*   **`const std::vector<std::pair<std::string, std::string>>& get() const`**: Returns the file information vector.

## Exceptions

CurlX uses a hierarchy of exceptions derived from `std::runtime_error` for error handling.

*   **`CurlX::RequestException`**: Base class for all CurlX-specific exceptions.
*   **`CurlX::ConnectionError`**: Thrown for network connection issues.
*   **`CurlX::Timeout`**: Thrown when a request times out.
*   **`CurlX::HTTPError`**: Thrown for 4xx or 5xx HTTP status codes (e.g., by `RESPONSE::raise_for_status()`).
*   **`CurlX::TooManyRedirects`**: Thrown when the maximum number of redirects is exceeded.
