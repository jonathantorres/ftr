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

add_subdirectory(src ${CMAKE_PROJECT_NAME})
add_subdirectory(tests ftr_tests)

