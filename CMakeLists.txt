project(NeoSDK)

cmake_minimum_required(VERSION 3.10)

option(BUILD_SHARED_LIBS ON)

find_package(GTest)
if(NOT GTEST_FOUND)
	set(NO_TESTS TRUE)
endif()

add_subdirectory(NeoCore)
add_subdirectory(NeoEngine)
add_subdirectory(Neo2D)
