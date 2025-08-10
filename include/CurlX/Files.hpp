#pragma once

#include <string>
#include <vector>
#include <utility>

namespace CurlX {

class FILES { // Changed to FILES
public:
    FILES() = default; // Changed to FILES
    FILES(std::vector<std::pair<std::string, std::string>> f) : files_vec(std::move(f)) {}
    FILES(const std::initializer_list<std::pair<std::string, std::string>>& init_list) : files_vec(init_list.begin(), init_list.end()) {}

    const std::vector<std::pair<std::string, std::string>>& get() const { return files_vec; }

private:
    std::vector<std::pair<std::string, std::string>> files_vec;
};

} // namespace CurlX
