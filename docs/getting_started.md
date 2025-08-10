# Getting Started with CurlX

This guide will walk you through setting up CurlX and making your first HTTP request.

## Installation

CurlX uses CMake for building and depends on `libcurl` and `nlohmann/json`.

### Prerequisites

Before you begin, ensure you have the following installed:

*   **CMake**: Version 3.15 or higher.
*   **C++ Compiler**: A C++17 compatible compiler (e.g., GCC, Clang, MSVC).
*   **libcurl**: Development libraries for `libcurl`. On Debian/Ubuntu, you can install it with `sudo apt-get install libcurl4-openssl-dev`. On Fedora, `sudo dnf install libcurl-devel`. On macOS with Homebrew, `brew install curl`.
*   **nlohmann/json**: This dependency will be fetched automatically by CMake.

### Building from Source

Follow these steps to clone, build, and install CurlX:

```bash
# 1. Clone the repository
git clone https://github.com/your-repo/CurlX.git # Replace with your actual repository URL
cd CurlX

# 2. Create a build directory and navigate into it
mkdir build
cd build

# 3. Configure the project with CMake
cmake ..

# 4. Build the library
make -j$(nproc) # Use all available CPU cores for faster compilation

# 5. (Optional) Run tests
make test

# 6. (Optional) Install the library (e.g., to /usr/local/lib, /usr/local/include)
sudo make install
```

## Basic Usage

CurlX provides a simple, chainable API for making HTTP requests. Here's a basic example of making a `GET` request:

```cpp
#include <CurlX/CurlX.hpp>
#include <iostream>

int main() {
    try {
        // Make a simple GET request to example.com
        CurlX::RESPONSE response = CurlX::GET("https://example.com");

        // Check if the request was successful (HTTP status 2xx)
        if (response.ok()) {
            std::cout << "Request successful!" << std::endl;
            std::cout << "Status Code: " << response.statusCode << std::endl;
            std::cout << "Response Body (first 200 chars): " << response.body.substr(0, 200) << "..." << std::endl;
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

### Making a POST Request

Here's an example demonstrating a `POST` request with a custom body and headers:

```cpp
#include <CurlX/CurlX.hpp>
#include <iostream>

int main() {
    try {
        // Define the target URL
        CurlX::URL httpbin("https://httpbin.org/post");

        // Create a request body (e.g., a string payload)
        CurlX::BODY payload("Hello from CurlX!");

        // Define custom headers
        CurlX::HEADERS headers;
        headers.add("Content-Type", "text/plain");
        headers.add("X-Custom-Header", "CurlX-Client");

        // Make a POST request
        CurlX::RESPONSE response = CurlX::POST(httpbin, payload, headers);

        // Check if the request was successful (HTTP status 2xx)
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

## Next Steps

*   Explore the [API Reference](api_reference.md) for a detailed breakdown of all classes and methods.
*   Learn about advanced features like authentication, proxies, and file uploads in [Advanced Usage](advanced.md).
*   Discover how you can contribute to CurlX in the [Contributing](CONTRIBUTING.md) guide.
