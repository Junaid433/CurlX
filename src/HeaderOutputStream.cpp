#include "CurlX/HeaderOutputStream.hpp"
#include "CurlX/Cookies.hpp"
#include <unordered_map>

namespace CurlX {
    std::ostream& operator<<(std::ostream& os, const HEADERS& headers) {
        for (const auto& header : headers.all()) {
            os << header << "\n";
        }
        return os;
    }

    std::ostream& operator<<(std::ostream& os, const COOKIES& cookies) {
        for (const auto& cookie_pair : cookies.all()) {
            os << cookie_pair.first << "=" << cookie_pair.second << "\n";
        }
        return os;
    }
}
