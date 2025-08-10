#pragma once

#include <ostream>
#include "Headers.hpp"
#include "Cookies.hpp"

namespace CurlX {
    std::ostream& operator<<(std::ostream& os, const HEADERS& headers);
    std::ostream& operator<<(std::ostream& os, const COOKIES& cookies);
}
