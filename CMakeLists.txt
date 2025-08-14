cmake_minimum_required(VERSION 3.20)
project(CurlX VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(CURL REQUIRED)

include(FetchContent)
FetchContent_Declare(
    json_lib
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)
set(JSON_BuildTests OFF CACHE INTERNAL "")
FetchContent_MakeAvailable(json_lib)

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

target_include_directories(CurlX PUBLIC include)
target_include_directories(CurlX PRIVATE ${json_lib_SOURCE_DIR}/include)
target_link_libraries(CurlX PUBLIC CURL::libcurl)
target_compile_options(CurlX PUBLIC -Wno-deprecated-literal-operator)

install(TARGETS CurlX
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/CurlX DESTINATION include)
install(DIRECTORY ${json_lib_SOURCE_DIR}/include/nlohmann/ DESTINATION include/nlohmann)