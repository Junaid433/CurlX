#include <gtest/gtest.h>
#include <CurlX/Cookies.hpp>
#include <string>
#include <unordered_map>
#include <optional>

TEST(CookiesTest, DefaultConstructor) {
    CurlX::COOKIES cookies;
    EXPECT_TRUE(cookies.all().empty());
}

TEST(CookiesTest, MapConstructor) {
    std::unordered_map<std::string, std::string> initial_cookies = {
        {"session_id", "abc123"},
        {"user_pref", "dark_mode"}
    };
    CurlX::COOKIES cookies(initial_cookies);
    EXPECT_EQ(cookies.all().size(), 2);
    EXPECT_EQ(cookies.get("session_id").value(), "abc123");
    EXPECT_EQ(cookies.get("user_pref").value(), "dark_mode");
}

TEST(CookiesTest, InitializerListConstructor) {
    CurlX::COOKIES cookies = {
        {"theme", "light"},
        {"lang", "en"}
    };
    EXPECT_EQ(cookies.all().size(), 2);
    EXPECT_EQ(cookies.get("theme").value(), "light");
    EXPECT_EQ(cookies.get("lang").value(), "en");
}

TEST(CookiesTest, AddCookie) {
    CurlX::COOKIES cookies;
    cookies.add("my_cookie", "my_value");
    EXPECT_EQ(cookies.all().size(), 1);
    EXPECT_EQ(cookies.get("my_cookie").value(), "my_value");
}

TEST(CookiesTest, AddMultipleCookies) {
    CurlX::COOKIES cookies;
    cookies.add("cookie1", "value1");
    cookies.add("cookie2", "value2");
    EXPECT_EQ(cookies.all().size(), 2);
    EXPECT_EQ(cookies.get("cookie1").value(), "value1");
    EXPECT_EQ(cookies.get("cookie2").value(), "value2");
}

TEST(CookiesTest, GetExistingCookie) {
    CurlX::COOKIES cookies;
    cookies.add("test_cookie", "test_value");
    std::optional<std::string> value = cookies.get("test_cookie");
    ASSERT_TRUE(value.has_value());
    EXPECT_EQ(value.value(), "test_value");
}

TEST(CookiesTest, GetNonExistingCookie) {
    CurlX::COOKIES cookies;
    std::optional<std::string> value = cookies.get("non_existent");
    EXPECT_FALSE(value.has_value());
}

TEST(CookiesTest, RemoveExistingCookie) {
    CurlX::COOKIES cookies;
    cookies.add("to_remove", "value");
    cookies.add("keep_this", "value");
    cookies.remove("to_remove");
    EXPECT_EQ(cookies.all().size(), 1);
    EXPECT_FALSE(cookies.get("to_remove").has_value());
    EXPECT_TRUE(cookies.get("keep_this").has_value());
}

TEST(CookiesTest, RemoveNonExistingCookie) {
    CurlX::COOKIES cookies;
    cookies.add("keep_this", "value");
    cookies.remove("non_existent");
    EXPECT_EQ(cookies.all().size(), 1);
    EXPECT_TRUE(cookies.get("keep_this").has_value());
}

TEST(CookiesTest, OverwriteCookie) {
    CurlX::COOKIES cookies;
    cookies.add("my_cookie", "original_value");
    cookies.add("my_cookie", "new_value"); // Adding with same key should overwrite
    EXPECT_EQ(cookies.all().size(), 1);
    EXPECT_EQ(cookies.get("my_cookie").value(), "new_value");
}

TEST(CookiesTest, AllMethod) {
    CurlX::COOKIES cookies;
    cookies.add("c1", "v1");
    cookies.add("c2", "v2");
    const auto& all_cookies = cookies.all();
    EXPECT_EQ(all_cookies.size(), 2);
    EXPECT_EQ(all_cookies.at("c1"), "v1");
    EXPECT_EQ(all_cookies.at("c2"), "v2");
}
