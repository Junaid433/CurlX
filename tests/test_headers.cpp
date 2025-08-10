#include <gtest/gtest.h>
#include <CurlX/Headers.hpp>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm> // For std::find_if

// Helper to check if a header exists in the vector returned by all()
bool containsHeader(const std::vector<std::string>& headers, const std::string& expected_header) {
    return std::find(headers.begin(), headers.end(), expected_header) != headers.end();
}

TEST(HeadersTest, DefaultConstructor) {
    CurlX::HEADERS headers;
    EXPECT_TRUE(headers.all().empty());
}

TEST(HeadersTest, VectorConstructor) {
    std::vector<std::string> initial_headers = {"Content-Type: application/json", "Accept: */*"};
    CurlX::HEADERS headers(initial_headers);
    EXPECT_EQ(headers.all().size(), 2);
    EXPECT_TRUE(containsHeader(headers.all(), "Content-Type: application/json"));
    EXPECT_TRUE(containsHeader(headers.all(), "Accept: */*"));
}

TEST(HeadersTest, UnorderedMapConstructor) {
    std::unordered_map<std::string, std::string> initial_map = {
        {"User-Agent", "CurlX"},
        {"Authorization", "Bearer token"}
    };
    CurlX::HEADERS headers(initial_map);
    EXPECT_EQ(headers.all().size(), 2);
    EXPECT_TRUE(containsHeader(headers.all(), "User-Agent: CurlX"));
    EXPECT_TRUE(containsHeader(headers.all(), "Authorization: Bearer token"));
}

TEST(HeadersTest, InitializerListConstructor) {
    CurlX::HEADERS headers = {
        {"X-Custom-Header", "Value1"},
        {"Another-Header", "Value2"}
    };
    EXPECT_EQ(headers.all().size(), 2);
    EXPECT_TRUE(containsHeader(headers.all(), "X-Custom-Header: Value1"));
    EXPECT_TRUE(containsHeader(headers.all(), "Another-Header: Value2"));
}

TEST(HeadersTest, AddKeyValue) {
    CurlX::HEADERS headers;
    headers.add("Content-Type", "application/xml");
    EXPECT_EQ(headers.all().size(), 1);
    EXPECT_TRUE(containsHeader(headers.all(), "Content-Type: application/xml"));
}

TEST(HeadersTest, AddHeaderLine) {
    CurlX::HEADERS headers;
    headers.add("Cache-Control: no-cache");
    EXPECT_EQ(headers.all().size(), 1);
    EXPECT_TRUE(containsHeader(headers.all(), "Cache-Control: no-cache"));
}

TEST(HeadersTest, AddMultipleHeaders) {
    CurlX::HEADERS headers;
    headers.add("Header1", "Value1");
    headers.add("Header2: Value2");
    headers.add("Header3", "Value3");
    EXPECT_EQ(headers.all().size(), 3);
    EXPECT_TRUE(containsHeader(headers.all(), "Header1: Value1"));
    EXPECT_TRUE(containsHeader(headers.all(), "Header2: Value2"));
    EXPECT_TRUE(containsHeader(headers.all(), "Header3: Value3"));
}

TEST(HeadersTest, GetExistingHeader) {
    CurlX::HEADERS headers;
    headers.add("X-Request-ID", "12345");
    std::optional<std::string> value = headers.get("X-Request-ID");
    ASSERT_TRUE(value.has_value());
    EXPECT_EQ(value.value(), "12345");
}

TEST(HeadersTest, GetNonExistingHeader) {
    CurlX::HEADERS headers;
    headers.add("X-Request-ID", "12345");
    std::optional<std::string> value = headers.get("Non-Existent");
    EXPECT_FALSE(value.has_value());
}

TEST(HeadersTest, GetCaseInsensitive) {
    CurlX::HEADERS headers;
    headers.add("Content-Type", "application/json");
    std::optional<std::string> value = headers.get("content-type"); // Should be case-insensitive
    ASSERT_TRUE(value.has_value());
    EXPECT_EQ(value.value(), "application/json");
}

TEST(HeadersTest, RemoveExistingHeader) {
    CurlX::HEADERS headers;
    headers.add("To-Remove", "Value");
    headers.add("Keep-This", "Value");
    headers.remove("To-Remove");
    EXPECT_EQ(headers.all().size(), 1);
    EXPECT_FALSE(containsHeader(headers.all(), "To-Remove: Value"));
    EXPECT_TRUE(containsHeader(headers.all(), "Keep-This: Value"));
}

TEST(HeadersTest, RemoveNonExistingHeader) {
    CurlX::HEADERS headers;
    headers.add("Keep-This", "Value");
    headers.remove("Non-Existent");
    EXPECT_EQ(headers.all().size(), 1);
    EXPECT_TRUE(containsHeader(headers.all(), "Keep-This: Value"));
}

TEST(HeadersTest, RemoveCaseInsensitive) {
    CurlX::HEADERS headers;
    headers.add("X-Test", "Value");
    headers.remove("x-test"); // Should be case-insensitive
    EXPECT_TRUE(headers.all().empty());
}

TEST(HeadersTest, AllMethod) {
    CurlX::HEADERS headers;
    headers.add("A", "1");
    headers.add("B", "2");
    const std::vector<std::string>& all_headers = headers.all();
    EXPECT_EQ(all_headers.size(), 2);
    EXPECT_TRUE(containsHeader(all_headers, "A: 1"));
    EXPECT_TRUE(containsHeader(all_headers, "B: 2"));
}

TEST(HeadersTest, RangeBasedForLoop) {
    CurlX::HEADERS headers = {{"H1", "V1"}, {"H2", "V2"}};
    std::vector<std::string> iterated_headers;
    for (const auto& header : headers) {
        iterated_headers.push_back(header);
    }
    EXPECT_EQ(iterated_headers.size(), 2);
    EXPECT_TRUE(containsHeader(iterated_headers, "H1: V1"));
    EXPECT_TRUE(containsHeader(iterated_headers, "H2: V2"));
}

// Note: Testing to_curl_slist() and free_curl_slist() would require linking against libcurl
// and is more of an integration test. For unit tests, we focus on the class's internal logic.
