## Find source and headers
file(GLOB_RECURSE HEADERS include/*.h)
file(GLOB_RECURSE SOURCES src/*.c*)

include(AddPlatformModule)

set(CMAKE_SHARED_LIBRARY_PREFIX "")
add_subdirectory(platform)

get_property(NEO_PLATFORM_LIBS GLOBAL PROPERTY NEO_PLATFORM_LIBS_PROP)
get_property(NEO_PLATFORM_SRCS GLOBAL PROPERTY NEO_PLATFORM_SRCS_PROP)
get_property(NEO_PLATFORM_INCS GLOBAL PROPERTY NEO_PLATFORM_INCS_PROP)

add_library(NeoEngine ${SOURCES} ${HEADERS} ${NEO_PLATFORM_SRCS})

if(NOT BUILD_SHARED_LIBS)
	target_compile_definitions(NeoCore PUBLIC -DNEO_ENGINE_STATIC)
endif()

target_compile_definitions(NeoEngine PRIVATE -DNEO_ENGINE_DLL)
target_include_directories(NeoEngine PUBLIC include ${NEO_PLATFORM_INCS})

if(NOT WII)
	set(DL_LIBS ${CMAKE_DL_LIBS})
endif()

target_link_libraries(NeoEngine PUBLIC NeoCore ${DL_LIBS} ${NEO_PLATFORM_LIBS})

if(NOT NO_TESTS)
	file(GLOB TEST_SOURCES tests/*.c*)
	add_executable(NeoEngineTest ${TEST_SOURCES})
	target_link_libraries(NeoEngineTest ${GTEST_LIBRARIES} NeoEngine)
endif()
