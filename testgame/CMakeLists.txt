
add_game(testgame src/main.cpp assets/)
target_link_libraries(testgame PUBLIC NeoEngine NeoHTML NeoStates)

configure_file(assets/test.html ${CMAKE_BINARY_DIR}/bin/assets/test.html)
configure_file(assets/test.dae ${CMAKE_BINARY_DIR}/bin/assets/test.dae)
file(COPY assets/test.png DESTINATION ${CMAKE_BINARY_DIR}/bin/assets)
file(COPY assets/Splash1.png DESTINATION ${CMAKE_BINARY_DIR}/bin/assets)
file(COPY assets/Splash2.png DESTINATION ${CMAKE_BINARY_DIR}/bin/assets)

file(COPY assets/Roboto-Regular.ttf DESTINATION ${CMAKE_BINARY_DIR}/bin/assets)
file(COPY assets/sans-serif.ttf DESTINATION ${CMAKE_BINARY_DIR}/bin/assets)
file(COPY assets/bootstrap.css DESTINATION ${CMAKE_BINARY_DIR}/bin/assets)
