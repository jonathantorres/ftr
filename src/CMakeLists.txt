set(SOURCES
    conf.cpp
    file_data_line.cpp
    log.cpp
    server.cpp
    session.cpp
    util.cpp
)

add_executable(${CMAKE_PROJECT_NAME} ${CMAKE_PROJECT_NAME}.cpp ${SOURCES})
install(TARGETS ${CMAKE_PROJECT_NAME} DESTINATION bin)
install(FILES ../${CMAKE_PROJECT_NAME}.conf DESTINATION .)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE pthread)
