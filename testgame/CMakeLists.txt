
## In external games this would happen one time only at project creation
## or each time the engine gets updated
copy_directory(${CMAKE_CURRENT_SOURCE_DIR}/../NeoEngine/platform/graphics/opengl/materials
		${CMAKE_CURRENT_SOURCE_DIR}/assets/shaders
		copy-testgame-shaders)

add_game(testgame src/main.cpp assets/)
add_dependencies(testgame copy-testgame-shaders)
