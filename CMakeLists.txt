cmake_minimum_required(VERSION 3.18.4)
project(ftr
    VERSION 0.0.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS False)

include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.1.0 # or later
)

FetchContent_Declare(
    string
    GIT_REPOSITORY https://github.com/jonathantorres/string.git
    GIT_TAG 12a52334a155f794a84d1abd2d8bf5171d39d252
)

FetchContent_MakeAvailable(Catch2 string)

add_compile_options(-Wall -Wextra -Wpedantic -Werror)
include_directories(src)

add_subdirectory(src ${CMAKE_PROJECT_NAME})
add_subdirectory(tests ftr_tests)

