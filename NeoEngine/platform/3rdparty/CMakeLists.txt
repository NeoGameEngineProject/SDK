
add_subdirectory(zlib-external EXCLUDE_FROM_ALL)
set(ZLIB_FOUND TRUE PARENT_SCOPE)
set(ZLIB_LIBRARY zlibstatic PARENT_SCOPE)
set(ZLIB_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/zlib-external/ ${CMAKE_CURRENT_SOURCE_DIR}/zlib-external/ PARENT_SCOPE)

#find_package(Freetype)
#if(NOT FREETYPE_FOUND)
	file(GLOB_RECURSE FREETYPE2_SRC freetype2-external/src/*.c*)
	set(FREETYPE_FOUND TRUE PARENT_SCOPE)
	set(FREETYPE_LIBRARIES freetype PARENT_SCOPE)
	set(FREETYPE_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/freetype2-external/include PARENT_SCOPE)

	add_subdirectory(freetype2-external)
#endif()
