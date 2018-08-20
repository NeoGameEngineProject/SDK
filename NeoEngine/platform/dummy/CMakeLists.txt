file(GLOB SOURCES src/*.c*)
file(GLOB INCLUDES include/*.h)

add_platform_module(
	SOURCES ${SOURCES} ${INCLUDES}
	INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/../../include
	LIBRARIES)
	
