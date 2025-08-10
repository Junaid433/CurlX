add_executable(CurlX_tests
    test_url.cpp
    test_headers.cpp
    test_cookies.cpp
    # Add more test files here as they are created
)

target_link_libraries(CurlX_tests
    PRIVATE
        CurlX
        gtest_main
        gtest
)

target_include_directories(CurlX_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../include # Include CurlX headers
)

include(GoogleTest)
gtest_discover_tests(CurlX_tests)