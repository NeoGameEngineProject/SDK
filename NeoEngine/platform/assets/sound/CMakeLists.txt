file(GLOB SOURCES src/*.c*)
file(GLOB INCLUDES include/*.h)

if(NOT EMSCRIPTEN)
	message("-- Setting OpenAL-Soft libtype to static. CHANGE TO SHARED FOR PROPRIETARY PROJECTS!")
	set(LIBTYPE "STATIC")
	set(ALSOFT_UTILS CACHE OFF "")
	set(ALSOFT_EXAMPLES CACHE OFF "")
	set(ALSOFT_TESTS CACHE OFF "")
	add_subdirectory(openal-soft-external EXCLUDE_FROM_ALL)
	
	add_platform_module(
		SOURCES ${SOURCES} ${INCLUDES}
		INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/../../include ${CMAKE_CURRENT_SOURCE_DIR}/openal-soft-external
		LIBRARIES OpenAL)
else()
	add_platform_module(
		SOURCES ${SOURCES} ${INCLUDES}
		INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/../../include ${CMAKE_CURRENT_SOURCE_DIR}/openal-soft-external
		LIBRARIES "")
endif()
