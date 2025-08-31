#include "CurlX/CurlX.hpp"
#include <iostream>
#include <cassert>

using namespace CurlX;

void test_headers_basic() {
    std::cout << "Testing basic headers functionality..." << std::endl;
    
    HEADERS headers;
    
    // Test adding headers
    headers.add("Content-Type", "application/json");
    headers.add("Authorization", "Bearer token123");
    
    assert(headers.size() == 2);
    assert(!headers.empty());
    
    // Test header retrieval
    auto content_type = headers.get("Content-Type");
    assert(content_type && *content_type == "application/json");
    
    auto auth = headers.get("Authorization");
    assert(auth && *auth == "Bearer token123");
    
    // Test case-insensitive search
    assert(headers.has("content-type"));
    assert(headers.has("CONTENT-TYPE"));
    
    std::cout << "✓ Basic headers test passed" << std::endl;
}

void test_headers_validation() {
    std::cout << "Testing headers validation..." << std::endl;
    
    HEADERS headers;
    
    // Test valid headers
    headers.add("Valid-Header", "valid-value");
    assert(headers.is_valid());
    
    // Test header limits
    for (int i = 0; i < 50; ++i) {
        headers.add("Header-" + std::to_string(i), "value-" + std::to_string(i));
    }
    
    assert(headers.size() <= 1000); // Should respect MAX_HEADERS_COUNT
    
    std::cout << "✓ Headers validation test passed" << std::endl;
}

void test_session_basic() {
    std::cout << "Testing basic session functionality..." << std::endl;
    
    Session session;
    
    // Test session validity
    assert(session.is_valid());
    
    // Test performance settings
    session.set_connection_timeout(5.0);
    session.set_transfer_timeout(10.0);
    session.set_max_connections_per_host(15);
    
    // Test default values
    assert(session.get_request_count() == 0);
    assert(session.get_average_response_time() == 0.0);
    
    std::cout << "✓ Basic session test passed" << std::endl;
}

void test_response_basic() {
    std::cout << "Testing basic response functionality..." << std::endl;
    
    RESPONSE response;
    
    // Test default values
    assert(response.statusCode == 0);
    assert(response.body.empty());
    assert(response.headers.empty());
    assert(response.history.empty());
    
    // Test empty response
    assert(response.body.empty());
    
    std::cout << "✓ Basic response test passed" << std::endl;
}

void test_url_basic() {
    std::cout << "Testing basic URL functionality..." << std::endl;
    
    URL url("https://example.com/path?param=value");
    
    // Test URL construction
    assert(!url.toString().empty());
    
    std::cout << "✓ Basic URL test passed" << std::endl;
}

void test_method_basic() {
    std::cout << "Testing basic method functionality..." << std::endl;
    
    METHOD get_method = METHOD::GET;
    METHOD post_method = METHOD::POST;
    
    assert(get_method.value == "GET");
    assert(post_method.value == "POST");
    
    std::cout << "✓ Basic method test passed" << std::endl;
}

void test_params_basic() {
    std::cout << "Testing basic parameters functionality..." << std::endl;
    
    PARAMS params;
    
    // Test default constructor
    auto params_map = params.get();
    assert(params_map.empty());
    
    // Test with initializer list
    PARAMS params_with_data{{"key1", "value1"}, {"key2", "value2"}};
    auto params_data = params_with_data.get();
    assert(params_data.size() == 2);
    assert(params_data["key1"] == "value1");
    assert(params_data["key2"] == "value2");
    
    std::cout << "✓ Basic parameters test passed" << std::endl;
}

void test_cookies_basic() {
    std::cout << "Testing basic cookies functionality..." << std::endl;
    
    COOKIES cookies;
    
    // Test adding cookies
    cookies.add("session_id", "abc123");
    cookies.add("user_pref", "dark_mode");
    
    auto cookies_map = cookies.all();
    assert(cookies_map.size() == 2);
    assert(cookies_map["session_id"] == "abc123");
    assert(cookies_map["user_pref"] == "dark_mode");
    
    std::cout << "✓ Basic cookies test passed" << std::endl;
}

int main() {
    std::cout << "CurlX Unit Tests" << std::endl;
    std::cout << "================" << std::endl;
    
    try {
        test_headers_basic();
        test_headers_validation();
        test_session_basic();
        test_response_basic();
        test_url_basic();
        test_method_basic();
        test_params_basic();
        test_cookies_basic();
        
        std::cout << "\n=== All Unit Tests Passed ===" << std::endl;
        std::cout << "✓ No segfaults detected" << std::endl;
        std::cout << "✓ All basic functionality working" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Unit test suite failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unit test suite failed with unknown exception" << std::endl;
        return 1;
    }
    
    return 0;
}
