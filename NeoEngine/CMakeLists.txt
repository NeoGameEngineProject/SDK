## Find source and headers
file(GLOB_RECURSE HEADERS include/*.h)
file(GLOB_RECURSE SOURCES src/*.c*)

add_subdirectory(platform)

set(CMAKE_SHARED_LIBRARY_PREFIX "")
add_library(NeoEngine ${SOURCES} ${HEADERS})

target_compile_definitions(NeoEngine PRIVATE -DNEO_CORE_DLL)
target_include_directories(NeoEngine PUBLIC include)

if(NOT WII)
set(DL_LIBS ${CMAKE_DL_LIBS})
endif()

target_link_libraries(NeoEngine PUBLIC NeoCore ${DL_LIBS} platform)

if(NOT NO_TESTS)
	file(GLOB TEST_SOURCES tests/*.c*)
	add_executable(NeoEngineTest ${TEST_SOURCES})
	target_link_libraries(NeoEngineTest ${GTEST_LIBRARIES} NeoEngine)
endif()

