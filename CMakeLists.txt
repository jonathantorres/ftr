cmake_minimum_required(VERSION 3.13.4)
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
    GIT_TAG 963d95353d2db2bc5d1b8f21502f3d8095b2614f
)

FetchContent_MakeAvailable(Catch2 string)

add_compile_options(-Wall -Wextra -Wpedantic -Werror)
include_directories(src)

add_subdirectory(src ${CMAKE_PROJECT_NAME})
add_subdirectory(tests ftr_tests)

