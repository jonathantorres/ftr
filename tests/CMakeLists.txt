# TODO: include the Catch testing framework dependency
# TODO: run the test binary right after it's built
add_executable(conf_test
    conf_test.cpp
    ../src/conf.cpp
    ../src/util.cpp
)

add_executable(util_test
    util_test.cpp
    ../src/util.cpp
)
