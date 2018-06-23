
if(WII)
	add_definitions(-Dstrcasecmp=strcmp -Dstrncasecmp=strncmp)
	
	add_subdirectory(wii)
	add_subdirectory(assets/collada)
else()
	add_subdirectory(sdl2)
	add_subdirectory(graphics/bgfx)
	add_subdirectory(assets/assimp)
	add_subdirectory(assets/png)
	# add_subdirectory(assets/collada)
endif()
