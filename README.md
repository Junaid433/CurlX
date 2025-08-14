# CurlX: A Modern C++ HTTP Client

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Build Status](https://img.shields.io/badge/Build-Passing-brightgreen)](https://github.com/Junaid433/CurlX/actions) 

CurlX: CURL ON STEROIDS!

## ✨ Features

*   **Simple & Chainable API**: Make HTTP requests with minimal, readable code.
*   **Full HTTP Verb Support**: GET, POST, PUT, DELETE, PATCH, HEAD, OPTIONS.
*   **Session & Cookie Management**: Automatic handling of cookies and persistent sessions.
*   **Authentication**: Built-in support for various authentication schemes (e.g., Basic Auth).
*   **Proxy Support**: Easily configure HTTP/HTTPS proxies.
*   **Redirect Handling**: Automatic following of HTTP redirects.
*   **SSL/TLS Verification**: Control over certificate verification for secure connections.
*   **File Transfers**: Effortless file uploads and downloads.
*   **JSON Integration**: Seamless parsing of JSON responses using `nlohmann/json`.
*   **Robust Error Handling**: Clear exception handling for network and HTTP errors.

## 🚀 Getting Started

### Prerequisites

Before you begin, ensure you have the following installed:

*   **CMake**: Version 3.15 or higher.
*   **C++ Compiler**: A C++17 compatible compiler (e.g., GCC, Clang, MSVC).
*   **libcurl**: Development libraries for `libcurl`.
*   **nlohmann/json**: This dependency will be fetched automatically by CMake.

### Building from Source

Follow these steps to clone, build, and install CurlX:

```bash
git clone https://github.com/Junaid433/CurlX.git 
cd CurlX
mkdir build
cd build
cmake ..
make -j$(nproc)
sudo make install
```

## 💡 Usage Example

Making a `POST` request with CurlX is straightforward:

```cpp
#include <CurlX/CurlX.hpp>
#include <iostream>

int main() {
    try {
        CurlX::URL httpbin("https://httpbin.org/post");
        CurlX::BODY payload("Hello from CurlX!");
        CurlX::HEADERS headers;
        headers.add("Content-Type", "text/plain");
        headers.add("X-Custom-Header", "CurlX-Client");

        CurlX::RESPONSE response = CurlX::POST(httpbin, payload, headers);

        if (response.ok()) {
            std::cout << "Request successful!" << std::endl;
            std::cout << "Status Code: " << response.statusCode << std::endl;
            std::cout << "Response Body: " << response.body << std::endl;
        } else {
            std::cerr << "Request failed with status code: " << response.statusCode << std::endl;
            std::cerr << "Error Body: " << response.body << std::endl;
        }
    } catch (const CurlX::Exception& e) {
        std::cerr << "CurlX Exception: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Standard Exception: " << e.what() << std::endl;
    }
    return 0;
}
```

## 📚 API Overview

CurlX provides a clear and consistent API design:

*   **`CurlX::Client`**: The primary class for managing HTTP sessions, connection settings, and making requests.
*   **`CurlX::Request`**: Configures individual HTTP requests, including URL, method, headers, and body.
*   **`CurlX::Response`**: Encapsulates the server's response, providing access to status code, headers, body, and utility methods like `ok()`, `text()`, and `json()`.
*   **`CurlX::URL`**: Utility class for URL manipulation and parsing.
*   **`CurlX::Headers`**: Manages HTTP request and response headers.
*   **`CurlX::Body`**: Represents the request body.
*   **`CurlX::Exceptions`**: Custom exception classes for robust error handling.

For detailed API documentation and advanced usage patterns, please refer to the `docs/` directory.

## 📄 License

CurlX is distributed under the **MIT License**. See the [LICENSE](LICENSE) file for more details.
