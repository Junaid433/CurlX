#pragma once

#include <string>
#include <map>

namespace CurlX {

using StringMap = std::map<std::string, std::string>;

class PARAMS { // Changed to PARAMS
public:
    PARAMS() = default; // Changed to PARAMS
    PARAMS(StringMap p) : params_map(std::move(p)) {}
    PARAMS(const std::initializer_list<std::pair<std::string, std::string>>& init_list) : params_map(init_list.begin(), init_list.end()) {}

    const std::map<std::string, std::string>& get() const { return params_map; }

private:
    std::map<std::string, std::string> params_map;
};

} // namespace CurlX
