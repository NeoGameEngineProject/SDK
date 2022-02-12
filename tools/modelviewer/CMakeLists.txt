
add_executable(modelviewer main.cpp)
target_link_libraries(modelviewer PRIVATE NeoEngine)

install(TARGETS modelviewer RUNTIME DESTINATION bin)
