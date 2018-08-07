project(NeoSDK)
cmake_minimum_required(VERSION 3.10)

set(CMAKE_MODULE_PATH 
	${CMAKE_CURRENT_SOURCE_DIR}/CMake
	${CMAKE_MODULE_PATH}
)

set(CMAKE_CXX_STANDARD 14)
option(BUILD_SHARED_LIBS OFF)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(CMAKE_BUILD_TYPE STREQUAL "Release" AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	set(CMAKE_CXX_FLAGS "-flto -s ${CMAKE_CXX_FLAGS}")
	set(CMAKE_C_FLAGS "-flto -s ${CMAKE_C_FLAGS}")
	set(CMAKE_EXE_LINKER_FLAGS "-flto ${CMAKE_EXE_LINKER_FLAGS}")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release" AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" AND NOT EMSCRIPTEN)
	set(CMAKE_CXX_FLAGS "-flto=thin ${CMAKE_CXX_FLAGS}")
	set(CMAKE_C_FLAGS "-flto=thin ${CMAKE_C_FLAGS}")
	set(CMAKE_EXE_LINKER_FLAGS "-flto=thin -s ${CMAKE_EXE_LINKER_FLAGS}")
endif()

if(EMSCRIPTEN)
	include(EmscriptenConfig)
endif()

include(AddGame)

if(NOT NO_TESTS)
	find_package(GTest)
	if(NOT GTEST_FOUND)
		set(NO_TESTS TRUE)
	endif()
endif()

if(ENABLE_SANITIZERS)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address -fsanitize=undefined")
endif()

add_subdirectory(NeoCore)
add_subdirectory(NeoEngine)
add_subdirectory(NeoHTML)
add_subdirectory(NeoStates)
# add_subdirectory(NeoAI)
# add_subdirectory(Neo2D)

if(ENABLE_SANITIZERS)
	target_compile_options(NeoEngine PUBLIC -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer)
	target_compile_options(NeoCore PUBLIC -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer)
endif()

if(NOT NO_TESTGAME)
	add_subdirectory(testgame)
endif()

if(NOT NO_DOCS)
	add_subdirectory(Doc)
endif()

