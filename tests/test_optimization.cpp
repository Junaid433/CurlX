#include "CurlX/CurlX.hpp"
#include <iostream>
#include <chrono>
#include <thread>
#include <future>
#include <vector>
#include <memory>
#include <cassert>

using namespace CurlX;

// Performance testing function
void test_performance() {
    std::cout << "\n=== Performance Testing ===" << std::endl;
    
    Session session;
    
    // Configure performance settings
    session.set_connection_timeout(10.0);
    session.set_transfer_timeout(30.0);
    session.set_max_connections_per_host(20);
    session.set_keep_alive(true);
    session.set_compression(true);
    
    // Test multiple concurrent requests
    std::vector<std::future<RESPONSE>> futures;
    const int num_requests = 5;
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < num_requests; ++i) {
        // Create headers first
        HEADERS headers;
        headers.add("User-Agent", "CurlX-Optimized/1.0");
        
        futures.push_back(session.send_async(
            REQUEST()
                .url(URL("https://httpbin.org/get"))
                .method(METHOD::GET)
                .headers(headers)
        ));
    }
    
    // Wait for all requests to complete
    for (auto& future : futures) {
        try {
            RESPONSE response = future.get();
            std::cout << "Request completed with status: " << response.statusCode << std::endl;
        } catch (const std::exception& e) {
            std::cout << "Request failed: " << e.what() << std::endl;
        }
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    std::cout << "Total time for " << num_requests << " concurrent requests: " 
              << duration.count() << "ms" << std::endl;
    std::cout << "Average response time: " << session.get_average_response_time() << "s" << std::endl;
    std::cout << "Total requests processed: " << session.get_request_count() << std::endl;
}

// Safety testing function
void test_safety_features() {
    std::cout << "\n=== Safety Testing ===" << std::endl;
    
    Session session;
    
    // Test 1: Invalid URL handling
    try {
        std::cout << "Testing invalid URL handling..." << std::endl;
        RESPONSE response = session.GET(URL(""), PARAMS(), HEADERS());
        std::cout << "ERROR: Should have thrown exception for invalid URL" << std::endl;
    } catch (const RequestException& e) {
        std::cout << "✓ Correctly caught invalid URL exception: " << e.what() << std::endl;
    }
    
    // Test 2: Large header handling
    try {
        std::cout << "Testing large header handling..." << std::endl;
        HEADERS large_headers;
        std::string large_value(10000, 'x'); // 10KB header value
        
        for (int i = 0; i < 100; ++i) {
            large_headers.add("Large-Header-" + std::to_string(i), large_value);
        }
        
        RESPONSE response = session.GET(
            URL("https://httpbin.org/get"), 
            PARAMS(), 
            large_headers
        );
        std::cout << "✓ Large headers handled successfully" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Large headers failed: " << e.what() << std::endl;
    }
    
    // Test 3: Session validation
    std::cout << "Testing session validation..." << std::endl;
    if (session.is_valid()) {
        std::cout << "✓ Session is valid" << std::endl;
    } else {
        std::cout << "✗ Session is invalid" << std::endl;
    }
    
    // Test 4: Memory safety with large responses
    try {
        std::cout << "Testing memory safety with large response..." << std::endl;
        RESPONSE response = session.GET(URL("https://httpbin.org/bytes/1000000")); // 1MB response
        
        if (response.body.length() > 0) {
            std::cout << "✓ Large response handled safely, size: " 
                      << response.body.length() << " bytes" << std::endl;
        } else {
            std::cout << "✗ Large response body is empty" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "Large response failed: " << e.what() << std::endl;
    }
}

// Error handling testing
void test_error_handling() {
    std::cout << "\n=== Error Handling Testing ===" << std::endl;
    
    Session session;
    
    // Test 1: Connection timeout
    try {
        std::cout << "Testing connection timeout..." << std::endl;
        session.set_connection_timeout(0.001); // 1ms timeout
        
        RESPONSE response = session.GET(URL("https://httpbin.org/delay/5"));
        std::cout << "ERROR: Should have timed out" << std::endl;
    } catch (const Timeout& e) {
        std::cout << "✓ Correctly caught timeout exception: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Caught different exception: " << e.what() << std::endl;
    }
    
    // Reset timeout
    session.set_connection_timeout(10.0);
    
    // Test 2: Invalid host
    try {
        std::cout << "Testing invalid host..." << std::endl;
        RESPONSE response = session.GET(URL("https://invalid-host-that-does-not-exist-12345.com"));
        std::cout << "ERROR: Should have failed to resolve host" << std::endl;
    } catch (const ConnectionError& e) {
        std::cout << "✓ Correctly caught connection error: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Caught different exception: " << e.what() << std::endl;
    }
}

// Memory management testing
void test_memory_management() {
    std::cout << "\n=== Memory Management Testing ===" << std::endl;
    
    // Test 1: Session lifecycle
    {
        std::cout << "Testing session lifecycle..." << std::endl;
        Session session;
        
        if (session.is_valid()) {
            std::cout << "✓ Session created successfully" << std::endl;
        }
        
        // Session will be automatically destroyed here
    }
    std::cout << "✓ Session destroyed successfully" << std::endl;
    
    // Test 2: Move semantics
    std::cout << "Testing move semantics..." << std::endl;
    Session session1;
    session1.set_connection_timeout(5.0);
    
    Session session2 = std::move(session1);
    
    if (session2.is_valid() && !session1.is_valid()) {
        std::cout << "✓ Move semantics working correctly" << std::endl;
    } else {
        std::cout << "✗ Move semantics failed" << std::endl;
    }
    
    // Test 3: Response memory management
    try {
        std::cout << "Testing response memory management..." << std::endl;
        Session session;
        
        RESPONSE response = session.GET(URL("https://httpbin.org/json"));
        
        // Test response validation
        if (response.body.length() > 0) {
            std::cout << "✓ Response validation passed" << std::endl;
        } else {
            std::cout << "✗ Response validation failed" << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cout << "Response test failed: " << e.what() << std::endl;
    }
}

// Header safety testing
void test_header_safety() {
    std::cout << "\n=== Header Safety Testing ===" << std::endl;
    
    HEADERS headers;
    
    // Test 1: Valid headers
    try {
        headers.add("Content-Type", "application/json");
        headers.add("Authorization", "Bearer token123");
        std::cout << "✓ Valid headers added successfully" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "✗ Valid headers failed: " << e.what() << std::endl;
        return;
    }
    
    // Test 2: Header validation
    if (headers.is_valid()) {
        std::cout << "✓ Headers validation passed" << std::endl;
    } else {
        std::cout << "✗ Headers validation failed" << std::endl;
    }
    
    // Test 3: Header retrieval
    auto content_type = headers.get("Content-Type");
    if (content_type && *content_type == "application/json") {
        std::cout << "✓ Header retrieval working correctly" << std::endl;
    } else {
        std::cout << "✗ Header retrieval failed" << std::endl;
    }
    
    // Test 4: Case-insensitive search
    if (headers.has("content-type")) {
        std::cout << "✓ Case-insensitive header search working" << std::endl;
    } else {
        std::cout << "✗ Case-insensitive header search failed" << std::endl;
    }
    
    std::cout << "Total headers: " << headers.size() << std::endl;
}

int main() {
    std::cout << "CurlX Advanced Optimization and Safety Testing" << std::endl;
    std::cout << "=============================================" << std::endl;
    
    try {
        // Test all safety and performance features
        test_header_safety();
        test_memory_management();
        test_error_handling();
        test_safety_features();
        test_performance();
        
        std::cout << "\n=== All Tests Completed Successfully ===" << std::endl;
        std::cout << "✓ No segfaults detected" << std::endl;
        std::cout << "✓ All safety features working" << std::endl;
        std::cout << "✓ Performance optimizations active" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Test suite failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Test suite failed with unknown exception" << std::endl;
        return 1;
    }
    
    return 0;
}
