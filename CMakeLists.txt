cmake_minimum_required(VERSION 3.15)
project(CurlX VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POLICY_DEFAULT_CMP0048 NEW)

find_package(CURL REQUIRED)

# Fetch nlohmann_json (header-only)
include(FetchContent)
FetchContent_Declare(
  json_lib
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.3
)
set(JSON_BuildTests OFF CACHE INTERNAL "")
FetchContent_MakeAvailable(json_lib)

# Fetch GoogleTest (BUILD only)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.11.0
)
set(INSTALL_GTEST OFF CACHE INTERNAL "")
FetchContent_MakeAvailable(googletest)

# Core library
add_library(CurlX
    src/Headers.cpp
    src/Request.cpp
    src/HeaderOutputStream.cpp
    src/Get.cpp
    src/Post.cpp
    src/Put.cpp
    src/Delete.cpp
    src/Client.cpp
    src/Method.cpp
    src/Session.cpp
    src/Cookies.cpp
)

target_include_directories(CurlX PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Instead of linking nlohmann_json, add its include directory only
target_include_directories(CurlX PRIVATE
    ${json_lib_SOURCE_DIR}/include
)

target_link_libraries(CurlX PUBLIC CURL::libcurl)

target_compile_options(CurlX PUBLIC -Wno-deprecated-literal-operator)

# Install library and export targets
install(TARGETS CurlX
    EXPORT CurlXTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(EXPORT CurlXTargets
    FILE CurlXTargets.cmake
    NAMESPACE CurlX::
    DESTINATION lib/cmake/CurlX
)

# Version file for package config
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/CurlXConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/CurlXConfigVersion.cmake"
    DESTINATION lib/cmake/CurlX
)

install(FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/CurlXConfig.cmake.in"
    DESTINATION lib/cmake/CurlX
    RENAME CurlXConfig.cmake
)

# Install headers
install(DIRECTORY include/CurlX/ DESTINATION include)

# Tests
add_subdirectory(tests)
