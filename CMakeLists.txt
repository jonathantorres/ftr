cmake_minimum_required(VERSION 3.13.4)
project(ftrd
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS False)

add_compile_options(-Wall -Wextra -Wpedantic -Werror)

configure_file(
    ${PROJECT_SOURCE_DIR}/src/config.hpp.in
    ${PROJECT_SOURCE_DIR}/src/config.hpp
)

include_directories(src)
add_subdirectory(src ${CMAKE_PROJECT_NAME})

# don't build tests on release builds
if (NOT CMAKE_BUILD_TYPE STREQUAL "Release")
    add_subdirectory(tests ftr_tests)
endif()

