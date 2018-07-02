project(NeoSDK)
cmake_minimum_required(VERSION 3.10)

set(CMAKE_MODULE_PATH 
	${CMAKE_SOURCE_DIR}/CMake
	${CMAKE_MODULE_PATH}
)

option(BUILD_SHARED_LIBS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
if(CMAKE_BUILD_TYPE STREQUAL "Release" AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	set(CMAKE_CXX_FLAGS "-flto ${CMAKE_CXX_FLAGS}")
	set(CMAKE_C_FLAGS "-flto ${CMAKE_C_FLAGS}")
	set(CMAKE_EXE_LINKER_FLAGS "-flto ${CMAKE_EXE_LINKER_FLAGS}")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release" AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	set(CMAKE_CXX_FLAGS "-flto=thin ${CMAKE_CXX_FLAGS}")
	set(CMAKE_C_FLAGS "-flto=thin ${CMAKE_C_FLAGS}")
	set(CMAKE_EXE_LINKER_FLAGS "-flto=thin ${CMAKE_EXE_LINKER_FLAGS}")
endif()

find_package(GTest)
if(NOT GTEST_FOUND)
	set(NO_TESTS TRUE)
endif()

if(ENABLE_SANITIZERS)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address -fsanitize=undefined")
endif()

add_subdirectory(NeoCore)
add_subdirectory(NeoEngine)
add_subdirectory(NeoHTML)
add_subdirectory(NeoStates)

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

