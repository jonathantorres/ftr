cmake_minimum_required(VERSION 3.18.4)
project(ftr
    VERSION 0.0.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS False)

add_compile_options(-Wall -Wextra -Wpedantic -Werror)
include_directories(src)

set(SOURCES
    src/conf.cpp
    src/log.cpp
    src/server.cpp
    src/session.cpp
    src/util.cpp
)

add_executable("${CMAKE_PROJECT_NAME}" "src/${CMAKE_PROJECT_NAME}.cpp" "${SOURCES}")

# tests
add_executable(conf_test tests/conf_test.cpp "${SOURCES}")
add_executable(util_test tests/util_test.cpp "${SOURCES}")
