# TODO: run the test binary right after it's built
add_executable(conf_test
    conf_test.cpp
    ../src/conf.cpp
    ../src/util.cpp
)
target_link_libraries(conf_test PRIVATE Catch2::Catch2WithMain)

add_executable(util_test
    util_test.cpp
    ../src/util.cpp
)
target_link_libraries(util_test PRIVATE Catch2::Catch2WithMain)
