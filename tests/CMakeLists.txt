add_executable(conf_test
    conf_test.cpp
    ../src/conf.cpp
    ../src/util.cpp
)

# add_custom_command(TARGET conf_test
#     POST_BUILD
#     COMMAND ${CMAKE_CURRENT_BINARY_DIR}/conf_test
#     VERBATIM
# )

target_link_libraries(conf_test PRIVATE Catch2::Catch2WithMain PUBLIC string)

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
