#include <gtest/gtest.h>
#include <CurlX/Url.hpp>
#include <iostream> // For operator<< test

TEST(URLTest, DefaultConstructor) {
    CurlX::URL url;
    EXPECT_EQ(url.toString(), "");
}

TEST(URLTest, StringViewConstructor) {
    CurlX::URL url("https://example.com");
    EXPECT_EQ(url.toString(), "https://example.com");
}

TEST(URLTest, CStringConstructor) {
    CurlX::URL url("http://test.com");
    EXPECT_EQ(url.toString(), "http://test.com");
}

TEST(URLTest, AssignmentOperator) {
    CurlX::URL url;
    url = "https://new-example.com";
    EXPECT_EQ(url.toString(), "https://new-example.com");
}

TEST(URLTest, ToStringMethod) {
    CurlX::URL url("https://tostring.com");
    EXPECT_EQ(url.toString(), "https://tostring.com");
}

TEST(URLTest, CStrMethod) {
    CurlX::URL url("https://cstr.com");
    EXPECT_STREQ(url.c_str(), "https://cstr.com");
}

TEST(URLTest, StreamOperator) {
    CurlX::URL url("https://stream.com");
    std::stringstream ss;
    ss << url;
    EXPECT_EQ(ss.str(), "https://stream.com");
}

TEST(URLTest, EmptyURL) {
    CurlX::URL url("");
    EXPECT_EQ(url.toString(), "");
}

TEST(URLTest, LongURL) {
    std::string long_url = "https://www.example.com/path/to/resource?param1=value1&param2=value2#fragment";
    CurlX::URL url(long_url);
    EXPECT_EQ(url.toString(), long_url);
}

TEST(URLTest, URLWithSpecialCharacters) {
    CurlX::URL url("https://example.com/path with spaces/file.txt?q=hello world");
    EXPECT_EQ(url.toString(), "https://example.com/path with spaces/file.txt?q=hello world");
}

// Add more URL-specific tests as needed to reach 34+ tests in total.
// For now, this covers basic URL functionality.
