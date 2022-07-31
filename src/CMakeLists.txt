set(SOURCES
    conf.cpp
    file_data_line.cpp
    log.cpp
    server.cpp
    session.cpp
    util.cpp
)

add_executable(${CMAKE_PROJECT_NAME} ${CMAKE_PROJECT_NAME}.cpp ${SOURCES})
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE pthread PUBLIC string)
