file(GLOB SOURCES src/*.c*)
file(GLOB INCLUDES include/*.h)

if(NOT EMSCRIPTEN)
	message("-- Setting OpenAL-Soft libtype to static. CHANGE TO SHARED FOR PROPRIETARY PROJECTS!")
	set(LIBTYPE "STATIC" CACHE STRING "" FORCE)
	set(ALSOFT_UTILS OFF CACHE BOOL "")
	set(ALSOFT_EXAMPLES OFF CACHE BOOL "")
	set(ALSOFT_TESTS OFF CACHE BOOL "")
	add_subdirectory(openal-soft-external EXCLUDE_FROM_ALL)
	
	add_platform_module(
		SOURCES ${SOURCES} ${INCLUDES}
		INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/../../include ${CMAKE_CURRENT_SOURCE_DIR}/openal-soft-external/include
		LIBRARIES OpenAL)
else()
	add_platform_module(
		SOURCES ${SOURCES} ${INCLUDES}
		INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/../../include ${CMAKE_CURRENT_SOURCE_DIR}/openal-soft-external/include
		LIBRARIES "")
endif()
