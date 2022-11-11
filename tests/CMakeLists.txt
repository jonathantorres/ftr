include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.1.0 # or later
)

if (${CMAKE_VERSION} VERSION_LESS 3.14)
    FetchContent_GetProperties(Catch2)
    if (NOT Catch2_POPULATED)
        FetchContent_Populate(Catch2)
        add_subdirectory(${catch2_SOURCE_DIR} ${catch2_BINARY_DIR})
    endif()
else()
    FetchContent_MakeAvailable(Catch2)
endif()

add_executable(conf_test
    conf_test.cpp
    ../src/conf.cpp
    ../src/util.cpp
)

add_custom_command(TARGET conf_test
    POST_BUILD
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/conf_test
    VERBATIM
)

target_link_libraries(conf_test PRIVATE Catch2::Catch2WithMain PUBLIC xtd)

add_executable(util_test
    util_test.cpp
    ../src/util.cpp
)

add_custom_command(TARGET util_test
    POST_BUILD
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/util_test
    VERBATIM
)

target_link_libraries(util_test PRIVATE Catch2::Catch2WithMain)
