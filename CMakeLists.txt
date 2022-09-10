cmake_minimum_required(VERSION 3.13.4)
project(ftr
    VERSION 0.0.0
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

include(FetchContent)

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

FetchContent_Declare(
    daemon
    GIT_REPOSITORY https://github.com/jonathantorres/daemon.git
    GIT_TAG e04d1e6774643cc35328c471e264f8c33baa7a66
)

if (${CMAKE_VERSION} VERSION_LESS 3.14)
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

    FetchContent_GetProperties(daemon)
    if (NOT daemon_POPULATED)
        FetchContent_Populate(daemon)
        add_subdirectory(${daemon_SOURCE_DIR} ${daemon_BINARY_DIR})
    endif()
else()
    FetchContent_MakeAvailable(string cmd daemon)
endif()

include_directories(src)
add_subdirectory(src ${CMAKE_PROJECT_NAME})

# don't build tests on release builds
if (NOT CMAKE_BUILD_TYPE STREQUAL "Release")
    add_subdirectory(tests ftr_tests)
endif()

