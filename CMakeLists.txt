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

FetchContent_Declare(
    cmd
    GIT_REPOSITORY https://github.com/jonathantorres/cmd.git
    GIT_TAG 40ba67be6741327e35000816526441a6d12466ed
)

if (${CMAKE_VERSION} VERSION_LESS 3.14)
    FetchContent_GetProperties(Catch2)
    if (NOT Catch2_POPULATED)
        FetchContent_Populate(Catch2)
        add_subdirectory(${catch2_SOURCE_DIR} ${catch2_BINARY_DIR})
    endif()

    FetchContent_GetProperties(string)
    if (NOT string_POPULATED)
        FetchContent_Populate(string)
        add_subdirectory(${string_SOURCE_DIR} ${string_BINARY_DIR})
    endif()

    FetchContent_GetProperties(cmd)
    if (NOT cmd_POPULATED)
        FetchContent_Populate(cmd)
        add_subdirectory(${cmd_SOURCE_DIR} ${cmd_BINARY_DIR})
    endif()
else()
    FetchContent_MakeAvailable(Catch2 string cmd)
endif()

add_compile_options(-Wall -Wextra -Wpedantic -Werror)
include_directories(src)

add_subdirectory(src ${CMAKE_PROJECT_NAME})
add_subdirectory(tests ftr_tests)

