file(GLOB SOURCES src/*.c*)
file(GLOB INCLUDES include/*.h)

if(NOT EMSCRIPTEN)
	
	find_package(OpenAL)
	
	if(NOT OPENAL_FOUND)
		set(LIBTYPE "DYNAMIC" CACHE STRING "" FORCE)
		set(ALSOFT_UTILS OFF CACHE BOOL "")
		set(ALSOFT_EXAMPLES OFF CACHE BOOL "")
		set(ALSOFT_TESTS OFF CACHE BOOL "")
		
		add_subdirectory(openal-soft-external EXCLUDE_FROM_ALL)
		set(OPENAL_LIBRARY OpenAL)
		set(OPENAL_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/openal-soft-external/include)
	endif()
	
	
	add_platform_module(
		SOURCES ${SOURCES} ${INCLUDES}
		INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/../../include ${OPENAL_INCLUDE_DIR}
		LIBRARIES ${OPENAL_LIBRARY})
else()
	add_platform_module(
		SOURCES ${SOURCES} ${INCLUDES}
		INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/../../include ${CMAKE_CURRENT_SOURCE_DIR}/openal-soft-external/include
		LIBRARIES "")
endif()
