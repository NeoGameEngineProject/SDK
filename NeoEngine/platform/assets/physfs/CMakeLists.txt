file(GLOB SOURCES src/*.c*)
file(GLOB INCLUDES include/*.h)

set(PHYSFS_BUILD_SHARED CACHE "" OFF FORCE)
set(PHYSFS_BUILD_TEST CACHE "" OFF FORCE)
add_subdirectory(physfs-external EXCLUDE_FROM_ALL)

add_platform_module(
	SOURCES ${SOURCES} ${INCLUDES}
	INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/../../include ${CMAKE_CURRENT_SOURCE_DIR}/physfs-external/src
	LIBRARIES physfs-static)
