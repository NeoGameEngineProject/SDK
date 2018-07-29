file(GLOB SOURCES src/*.c*)
file(GLOB INCLUDES include/*.h)

if(NOT EMSCRIPTEN)
	message("-- Setting OpenAL-Soft libtype to static. CHANGE TO SHARED FOR PROPRIETARY PROJECTS!")
	set(LIBTYPE CACHE "STATIC" "" FORCE)
	set(ALSOFT_UTILS CACHE OFF "")
	set(ALSOFT_EXAMPLES CACHE OFF "")
	set(ALSOFT_TESTS CACHE OFF "")
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
