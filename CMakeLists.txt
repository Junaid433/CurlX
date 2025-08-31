cmake_minimum_required(VERSION 3.20)
project(CurlX VERSION 1.0 LANGUAGES CXX)

# Set C++ standard to latest for maximum optimization opportunities
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Safety-first optimization flags
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -march=native -mtune=native -flto -fuse-linker-plugin")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3 -DDEBUG -fsanitize=address,undefined -fno-omit-frame-pointer -Wall -Wextra -Wpedantic -Werror")

# Default to Release mode for maximum performance
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Enable link-time optimization for Release builds
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)

# Enable all warnings and treat them as errors for safety
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Werror")

# Enable additional safety checks
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fstack-protector-strong -fstack-clash-protection -D_FORTIFY_SOURCE=2")

# Enable performance optimizations
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-strict-aliasing -fno-delete-null-pointer-checks")

# Enable vectorization and SIMD optimizations
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2 -mfma -msse4.2")

# Enable function inlining and optimization hints
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -finline-functions -finline-small-functions -foptimize-sibling-calls")

find_package(CURL REQUIRED)

include(FetchContent)
FetchContent_Declare(
    json_lib
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)
set(JSON_BuildTests OFF CACHE INTERNAL "")
FetchContent_MakeAvailable(json_lib)

# Create optimized library target
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
    src/Response.cpp
)

# Set target-specific optimization flags
target_compile_options(CurlX PRIVATE
    $<$<CONFIG:Release>:-O3 -DNDEBUG -march=native -mtune=native>
    $<$<CONFIG:Debug>:-O0 -g3 -DDEBUG -fsanitize=address,undefined>
    $<$<CONFIG:RelWithDebInfo>:-O2 -g -DNDEBUG>
    $<$<CONFIG:MinSizeRel>:-Os -DNDEBUG>
)

# Enable interprocedural optimization for Release builds
set_target_properties(CurlX PROPERTIES
    INTERPROCEDURAL_OPTIMIZATION_RELEASE ON
    POSITION_INDEPENDENT_CODE ON
)

target_include_directories(CurlX PUBLIC include)
target_include_directories(CurlX PRIVATE ${json_lib_SOURCE_DIR}/include)
target_link_libraries(CurlX PUBLIC CURL::libcurl)

# Link optimization libraries
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_link_libraries(CurlX PRIVATE -lrt)
endif()

# Enable specific optimizations for the target
target_compile_features(CurlX PRIVATE cxx_std_23)

# Test targets
option(BUILD_TESTS "Build test executables" ON)

if(BUILD_TESTS)
    # Unit tests
    add_executable(curlx_unit_tests tests/test_unit.cpp)
    target_link_libraries(curlx_unit_tests PRIVATE CurlX)
    target_include_directories(curlx_unit_tests PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${json_lib_SOURCE_DIR}/include
    )
    
    # Integration tests
    add_executable(curlx_integration_tests tests/test_optimization.cpp)
    target_link_libraries(curlx_integration_tests PRIVATE CurlX)
    target_include_directories(curlx_integration_tests PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${json_lib_SOURCE_DIR}/include
    )
    
    # Test compilation flags
    target_compile_options(curlx_unit_tests PRIVATE
        -Wall -Wextra -Wpedantic
        -O2 -g
    )
    
    target_compile_options(curlx_integration_tests PRIVATE
        -Wall -Wextra -Wpedantic
        -O2 -g
    )
    
    # Add test targets to CTest
    enable_testing()
    add_test(NAME UnitTests COMMAND curlx_unit_tests)
    add_test(NAME IntegrationTests COMMAND curlx_integration_tests)
    
    # Custom test targets
    add_custom_target(test_unit
        COMMAND curlx_unit_tests
        DEPENDS curlx_unit_tests
        COMMENT "Running unit tests"
    )
    
    add_custom_target(test_integration
        COMMAND curlx_integration_tests
        DEPENDS curlx_integration_tests
        COMMENT "Running integration tests"
    )
    
    add_custom_target(test_all
        COMMAND curlx_unit_tests
        COMMAND curlx_integration_tests
        DEPENDS curlx_unit_tests curlx_integration_tests
        COMMENT "Running all tests"
    )
endif()

install(TARGETS CurlX
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/CurlX DESTINATION include)
install(DIRECTORY ${json_lib_SOURCE_DIR}/include/nlohmann/ DESTINATION include/nlohmann)

# Add custom target for performance testing
add_custom_target(perf_test
    COMMAND ${CMAKE_COMMAND} -E echo "Running performance tests..."
    COMMAND ${CMAKE_COMMAND} -E echo "Build type: ${CMAKE_BUILD_TYPE}"
    COMMAND ${CMAKE_COMMAND} -E echo "Compiler flags: ${CMAKE_CXX_FLAGS}"
    DEPENDS CurlX
)