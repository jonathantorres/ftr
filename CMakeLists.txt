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
    xtd
    GIT_REPOSITORY https://github.com/jonathantorres/xtd.git
    GIT_TAG 17366c048dee10d748b3c3fd7fea908c9716a899
)

if (${CMAKE_VERSION} VERSION_LESS 3.14)
    FetchContent_GetProperties(xtd)
    if (NOT xtd_POPULATED)
        FetchContent_Populate(xtd)
        add_subdirectory(${xtd_SOURCE_DIR} ${xtd_BINARY_DIR})
    endif()
else()
    FetchContent_MakeAvailable(xtd)
endif()

include_directories(src)
add_subdirectory(src ${CMAKE_PROJECT_NAME})

# don't build tests on release builds
if (NOT CMAKE_BUILD_TYPE STREQUAL "Release")
    add_subdirectory(tests ftr_tests)
endif()

