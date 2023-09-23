set(SOURCES
    command.cpp
    conf.cpp
    daemon.cpp
    file_data.cpp
    log.cpp
    server.cpp
    session.cpp
    string.cpp
    util.cpp
)

add_executable(${CMAKE_PROJECT_NAME} ${CMAKE_PROJECT_NAME}.cpp ${SOURCES})
install(TARGETS ${CMAKE_PROJECT_NAME} DESTINATION bin)
install(FILES ../${CMAKE_PROJECT_NAME}.conf DESTINATION .)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE pthread)
