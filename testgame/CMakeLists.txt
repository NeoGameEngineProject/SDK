
add_executable(testgame src/main.cpp)
target_link_libraries(testgame PUBLIC NeoEngine NeoHTML)

configure_file(assets/test.html ${CMAKE_BINARY_DIR}/bin/assets/test.html)
configure_file(assets/test.dae ${CMAKE_BINARY_DIR}/bin/assets/test.dae)
file(COPY assets/test.png DESTINATION ${CMAKE_BINARY_DIR}/bin/assets)
file(COPY assets/Roboto-Regular.ttf DESTINATION ${CMAKE_BINARY_DIR}/bin/assets)
file(COPY assets/sans-serif.ttf DESTINATION ${CMAKE_BINARY_DIR}/bin/assets)
file(COPY assets/bootstrap.css DESTINATION ${CMAKE_BINARY_DIR}/bin/assets)
