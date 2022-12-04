
add_executable(modelviewer main.cpp)
target_link_libraries(modelviewer PRIVATE NeoEngine)

add_dependencies(modelviewer Materials)
install(TARGETS modelviewer RUNTIME DESTINATION bin)
