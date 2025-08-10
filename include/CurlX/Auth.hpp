#pragma once

#include <string>
#include <utility>

namespace CurlX {

    enum class AuthType {
        None,
        Basic,
        Digest
    };

    class AUTH {
    public:
        AUTH() : type_(AuthType::None) {}

        // Constructor for Basic and Digest auth
        AUTH(std::string username, std::string password, AuthType type = AuthType::Basic)
            : username_(std::move(username)), password_(std::move(password)), type_(type) {}

        // Constructor for simple string (for backward compatibility, assumes Basic)
        explicit AUTH(std::string user_pass_string) : type_(AuthType::Basic) {
            size_t colon_pos = user_pass_string.find(":");
            if (colon_pos != std::string::npos) {
                username_ = user_pass_string.substr(0, colon_pos);
                password_ = user_pass_string.substr(colon_pos + 1);
            } else {
                username_ = user_pass_string;
                password_ = "";
            }
        }

        AuthType type() const { return type_; }
        const std::string& username() const { return username_; }
        const std::string& password() const { return password_; }

        // Helper to get user:pass string for CURLOPT_USERPWD
        std::string user_pass_string() const {
            if (type_ != AuthType::None) {
                return username_ + ":" + password_;
            }
            return "";
        }

    private:
        std::string username_;
        std::string password_;
        AuthType type_;
    };

} // namespace CurlX