
if(WII)
	add_definitions(-include ${CMAKE_CURRENT_SOURCE_DIR}/wii/src/Wii.h)
	
	add_subdirectory(wii)
	add_subdirectory(assets/png)
	add_subdirectory(assets/assimp)
	add_subdirectory(assets/zipvfs)
elseif(EMSCRIPTEN)
	add_subdirectory(sdl2)
	add_subdirectory(graphics/bgfx)
	add_subdirectory(assets/assimp)
	add_subdirectory(assets/png)
	add_subdirectory(assets/sound)
	
	add_subdirectory(assets/physfs)
else()
	# if(WIN32)
	add_subdirectory(3rdparty)
	# endif()

	if(ENABLE_DUMMY_PLATFORM)
		add_subdirectory(dummy)
	else()
		add_subdirectory(sdl2)
	endif()

	if(ENABLE_OPENGL_RENDERER)
		add_subdirectory(graphics/opengl)
	else()
		add_subdirectory(graphics/bgfx)
	endif()
	
	add_subdirectory(assets/assimp)
	add_subdirectory(assets/png)
	add_subdirectory(assets/sound)
	
	add_subdirectory(assets/physfs)
	
	# add_subdirectory(assets/zipvfs)
endif()
