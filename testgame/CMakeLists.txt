
add_executable(testgame src/main.cpp)
target_link_libraries(testgame PUBLIC NeoEngine)

configure_file(assets/test.dae ${CMAKE_BINARY_DIR}/bin/assets/test.dae)
