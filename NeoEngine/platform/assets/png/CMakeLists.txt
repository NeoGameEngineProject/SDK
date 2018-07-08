file(GLOB SOURCES src/*.c*)
file(GLOB INCLUDES include/*.h)

add_subdirectory(libpng-external EXCLUDE_FROM_ALL)

add_platform_module(
	SOURCES ${SOURCES} ${INCLUDES}
	INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/../../include
		${CMAKE_CURRENT_SOURCE_DIR}/libpng-external/
		${CMAKE_CURRENT_BINARY_DIR}/libpng-external/
	LIBRARIES png_static)
	
