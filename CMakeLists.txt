project(NeoSDK)
cmake_minimum_required(VERSION 3.10)

set(CMAKE_MODULE_PATH 
	${CMAKE_SOURCE_DIR}/CMake
	${CMAKE_MODULE_PATH}
)

option(BUILD_SHARED_LIBS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(NOT NO_TESTGAME)
	add_subdirectory(testgame)
endif()

find_package(GTest)
if(NOT GTEST_FOUND)
	set(NO_TESTS TRUE)
endif()

add_subdirectory(NeoCore)
add_subdirectory(NeoEngine)
# add_subdirectory(Neo2D)
