
if(WII)
	add_definitions(-include ${CMAKE_CURRENT_SOURCE_DIR}/wii/src/Wii.h)
	
	add_subdirectory(wii)
	add_subdirectory(assets/png)
	add_subdirectory(assets/assimp)
	add_subdirectory(assets/zipvfs)
elseif(EMSCRIPTEN)
	add_subdirectory(sdl2)
	#add_subdirectory(graphics/bgfx)
	add_subdirectory(graphics/opengl)
	add_subdirectory(assets/assimp)
	add_subdirectory(assets/png)
	add_subdirectory(assets/tinygltf)
	add_subdirectory(assets/sound)
	add_subdirectory(assets/physfs)
	add_subdirectory(bullet)

	set(FLATBUFFERS_BUILD_TESTS FALSE CACHE BOOL "" FORCE)
	set(FLATBUFFERS_BUILD_FLATC TRUE CACHE BOOL "" FORCE)

	add_subdirectory(3rdparty/flatbuffers-external)
else()

	# For freetype etc.
	set(DISABLE_FORCE_DEBUG_POSTFIX TRUE)

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
	
	add_subdirectory(assets/tinygltf)
	add_subdirectory(assets/assimp)
	add_subdirectory(assets/png)
	add_subdirectory(assets/sound)
	add_subdirectory(assets/physfs)
	add_subdirectory(bullet)
	
	# add_subdirectory(assets/zipvfs)
endif()
