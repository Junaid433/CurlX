# Advanced Usage of CurlX

This document delves into more advanced features and configurations available in CurlX.

## Authentication

CurlX provides mechanisms to handle various forms of HTTP authentication.

### Basic Authentication

You can set basic authentication credentials directly on your request:

```cpp
#include <CurlX/CurlX.hpp>
#include <iostream>

int main() {
    try {
        CurlX::URL url("https://api.example.com/protected");
        CurlX::Auth auth("username", "password"); // Basic Auth

        CurlX::RESPONSE response = CurlX::GET(url, auth);

        if (response.ok()) {
            std::cout << "Authenticated GET successful!" << std::endl;
            std::cout << "Response: " << response.body << std::endl;
        } else {
            std::cerr << "Authentication failed or request error: " << response.statusCode << std::endl;
        }
    } catch (const CurlX::Exception& e) {
        std::cerr << "CurlX Exception: " << e.what() << std::endl;
    }
    return 0;
}
```

## Proxy Support

CurlX allows you to route your HTTP requests through a proxy server.

```cpp
#include <CurlX/CurlX.hpp>
#include <iostream>

int main() {
    try {
        CurlX::URL url("https://example.com");
        CurlX::Proxy proxy("http://your.proxy.com:8080"); // HTTP proxy
        // CurlX::Proxy proxy("socks5://your.socks.proxy.com:1080"); // SOCKS5 proxy

        CurlX::RESPONSE response = CurlX::GET(url, proxy);

        if (response.ok()) {
            std::cout << "Request via proxy successful!" << std::endl;
            std::cout << "Response: " << response.body.substr(0, 200) << "..." << std::endl;
        } else {
            std::cerr << "Request via proxy failed: " << response.statusCode << std::endl;
        }
    } catch (const CurlX::Exception& e) {
        std::cerr << "CurlX Exception: " << e.what() << std::endl;
    }
    return 0;
}
```

## Redirect Handling

By default, CurlX follows HTTP redirects. You can control this behavior.

```cpp
#include <CurlX/CurlX.hpp>
#include <iostream>

int main() {
    try {
        CurlX::URL url("http://httpbin.org/redirect/3"); // Redirects 3 times

        // Follow redirects (default behavior)
        CurlX::RESPONSE response_followed = CurlX::GET(url);
        std::cout << "Followed redirects. Final URL: " << response_followed.url << std::endl;

        // Disable redirects
        CurlX::Redirects no_redirects(0); // Do not follow any redirects
        CurlX::RESPONSE response_no_follow = CurlX::GET(url, no_redirects);
        std::cout << "Did not follow redirects. Status: " << response_no_follow.statusCode << std::endl;

    } catch (const CurlX::Exception& e) {
        std::cerr << "CurlX Exception: " << e.what() << std::endl;
    }
    return 0;
}
```

## File Uploads

CurlX supports uploading files as part of a `POST` or `PUT` request.

```cpp
#include <CurlX/CurlX.hpp>
#include <iostream>
#include <fstream> // For creating a dummy file

int main() {
    // Create a dummy file for upload
    std::ofstream ofs("upload_test.txt");
    ofs << "This is a test file for CurlX upload." << std::endl;
    ofs.close();

    try {
        CurlX::URL url("https://httpbin.org/post");
        CurlX::Files files;
        files.add("file_to_upload", "upload_test.txt", "text/plain"); // Field name, file path, content type

        CurlX::RESPONSE response = CurlX::POST(url, files);

        if (response.ok()) {
            std::cout << "File upload successful!" << std::endl;
            std::cout << "Response: " << response.body << std::endl;
        } else {
            std::cerr << "File upload failed: " << response.statusCode << std::endl;
            std::cerr << "Error Body: " << response.body << std::endl;
        }
    } catch (const CurlX::Exception& e) {
        std::cerr << "CurlX Exception: " << e.what() << std::endl;
    }

    // Clean up dummy file
    std::remove("upload_test.txt");
    return 0;
}
```

## SSL/TLS Verification

You can control whether CurlX verifies the SSL certificate of the server. It's highly recommended to keep verification enabled in production environments.

```cpp
#include <CurlX/CurlX.hpp>
#include <iostream>

int main() {
    try {
        CurlX::URL url("https://self-signed.badssl.com/"); // A site with a self-signed cert

        // Default behavior: SSL verification is enabled (will likely fail for badssl.com)
        // CurlX::RESPONSE response_default = CurlX::GET(url);

        // Disable SSL verification (use with caution!)
        CurlX::Verify no_verify(false);
        CurlX::RESPONSE response_no_verify = CurlX::GET(url, no_verify);

        if (response_no_verify.ok()) {
            std::cout << "Request with SSL verification disabled successful!" << std::endl;
            std::cout << "Response: " << response_no_verify.body.substr(0, 100) << "..." << std::endl;
        } else {
            std::cerr << "Request with SSL verification disabled failed: " << response_no_verify.statusCode << std::endl;
        }
    } catch (const CurlX::Exception& e) {
        std::cerr << "CurlX Exception: " << e.what() << std::endl;
    }
    return 0;
}
```
