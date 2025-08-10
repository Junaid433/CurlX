cmake_minimum_required(VERSION 3.15)
project(CurlX VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_POLICY_DEFAULT_CMP0048 NEW)

find_package(CURL REQUIRED)

# Fetch nlohmann_json
include(FetchContent)
FetchContent_Declare(
  json_lib
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(json_lib)

# Fetch GoogleTest
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.11.0
)
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

# libcurl is public so consumers get it automatically
target_link_libraries(CurlX PUBLIC CURL::libcurl)

# JSON is private — avoids export error
target_link_libraries(CurlX PRIVATE nlohmann_json::nlohmann_json)

target_compile_options(CurlX PUBLIC -Wno-deprecated-literal-operator)

# Install library
install(TARGETS CurlX
    EXPORT CurlXTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

# Export targets
install(EXPORT CurlXTargets
    FILE CurlXTargets.cmake
    NAMESPACE CurlX::
    DESTINATION lib/cmake/CurlX
)

# Version file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/CurlXConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/CurlXConfigVersion.cmake"
    DESTINATION lib/cmake/CurlX
)

install(
    FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/CurlXConfig.cmake.in"
    DESTINATION lib/cmake/CurlX
    RENAME CurlXConfig.cmake
)

# Install headers
install(DIRECTORY include/CurlX/ DESTINATION include)

# Tests
add_subdirectory(tests)
