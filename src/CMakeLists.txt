set(SOURCES
    conf.cpp
    log.cpp
    server.cpp
    session.cpp
    util.cpp
)

add_executable(${CMAKE_PROJECT_NAME} ${CMAKE_PROJECT_NAME}.cpp ${SOURCES})
target_link_libraries(${CMAKE_PROJECT_NAME} pthread)
