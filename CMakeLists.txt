cmake_minimum_required(VERSION 3.18.4)
project(ftr
    VERSION 0.0.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS False)
set(SRC_DIR "src")

add_compile_options(-Wall -Wextra -Wpedantic -Werror)
include_directories("${SRC_DIR}")

set(SOURCE_FILES
    conf
    log
    server
    session
    util
)

set(SOURCES "")

list(TRANSFORM SOURCE_FILES PREPEND "${SRC_DIR}/" OUTPUT_VARIABLE SOURCES)
list(TRANSFORM SOURCES APPEND .cpp)

add_executable("${CMAKE_PROJECT_NAME}" "${SRC_DIR}/${CMAKE_PROJECT_NAME}.cpp" "${SOURCES}")

# tests
add_executable(conf_test tests/conf_test.cpp "${SOURCES}")
add_executable(util_test tests/util_test.cpp "${SOURCES}")
