
cmake_policy(SET CMP0091 NEW)

project(Neo C CXX)
cmake_minimum_required(VERSION 3.10)

if(WIN32 AND NOT NEO_BUILD_SHARED_LIBS)
	set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

set(CMAKE_MODULE_PATH 
	${CMAKE_CURRENT_SOURCE_DIR}/CMake
	${CMAKE_MODULE_PATH}
)

SET(NEO_MAJOR_VERSION 0)
SET(NEO_MINOR_VERSION 6)
SET(NEO_PATCH_VERSION 0)

set(NEO_PACKAGE "Neo")
set(NEO_VENDOR "Yannick Pflanzer")
set(NEO_VERSION "${NEO_MAJOR_VERSION}.${NEO_MINOR_VERSION}.${NEO_PATCH_VERSION}")
set(NEO_COPYRIGHT "Copyright (c) 2014-2018, ${NEO_VENDOR}.")

set(NEO_CMAKE_EXPORT "lib/cmake")
set(CMAKE_INSTALL_LIBDIR "lib") # To prevent GNUInstallDirs from creating lib64

set(CMAKE_DEBUG_POSTFIX "" CACHE STRING "" FORCE)

include(CopyFiles)
include(ExternalProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(SANITIZER "address" CACHE STRING "The sanitizer to use")

add_definitions(-D_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING)

if(NEO_BUILD_SHARED_LIBS)
	set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)
endif()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	if(CMAKE_BUILD_TYPE STREQUAL "Release" AND NOT NO_LTO)
		set(CMAKE_CXX_FLAGS "-flto -s ${CMAKE_CXX_FLAGS}")
		set(CMAKE_C_FLAGS "-flto -s ${CMAKE_C_FLAGS}")
		set(CMAKE_EXE_LINKER_FLAGS "-flto ${CMAKE_EXE_LINKER_FLAGS}")
		set(CMAKE_SHARED_LINKER_FLAGS "-flto ${CMAKE_SHARED_LINKER_FLAGS}")
	endif()

	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-rpath,'\\$ORIGIN'")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-rpath,'\\$ORIGIN'")

	message("-- RPATH: ${CMAKE_SHARED_LINKER_FLAGS}")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" )
	if(CMAKE_BUILD_TYPE STREQUAL "Release" AND NOT EMSCRIPTEN AND NOT NO_LTO)
		set(CMAKE_CXX_FLAGS "-flto=thin ${CMAKE_CXX_FLAGS}")
		set(CMAKE_C_FLAGS "-flto=thin ${CMAKE_C_FLAGS}")
		set(CMAKE_EXE_LINKER_FLAGS "-flto=thin -s ${CMAKE_EXE_LINKER_FLAGS}")
		set(CMAKE_SHARED_LINKER_FLAGS "-flto=thin -s ${CMAKE_SHARED_LINKER_FLAGS}")
	endif()

	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-rpath,'\\$ORIGIN'")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-rpath,'\\$ORIGIN'")
endif()

if(EMSCRIPTEN)
	include(EmscriptenConfig)
endif()

include(AddGame)
include(PreProcessor)

if(NOT NO_TESTS)
	find_package(GTest)
	if(NOT GTEST_FOUND)
		set(NO_TESTS TRUE)
	endif()
endif()

option(ENABLE_OPENGL_RENDERER "Use the OpenGL graphics renderer by default." ON)
if(ENABLE_OPENGL_RENDERER)
	add_definitions(-DNEO_OPENGL=1)
endif()

add_subdirectory(NeoCore)
add_subdirectory(NeoEngine)
add_subdirectory(NeoHTML)
add_subdirectory(NeoStates)
add_subdirectory(NeoScript)

if(NOT NO_NET)
	add_subdirectory(NeoNet)
endif()

if(NOT APPLE AND NOT NO_VR)
	add_subdirectory(NeoVR)
endif()

if(NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	if(ENABLE_SANITIZERS)
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=${SANITIZER} -fsanitize=undefined")
		set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fsanitize=${SANITIZER} -fsanitize=undefined")
	endif()
		
	if(ENABLE_SANITIZERS)
		target_compile_options(NeoEngine PUBLIC -fsanitize=${SANITIZER} -fsanitize=undefined -fno-omit-frame-pointer)
		target_compile_options(NeoCore PUBLIC -fsanitize=${SANITIZER} -fsanitize=undefined -fno-omit-frame-pointer)
	endif()
endif()

if(DISABLE_MULTITHREAD)
	target_compile_definitions(NeoEngine PUBLIC -DNEO_SINGLE_THREAD=1)
endif()

# add_subdirectory(NeoAI)
# add_subdirectory(Neo2D)

if(NOT NO_TESTGAME)
	add_subdirectory(testgame)
endif()

if(NOT NO_DOCS)
	add_subdirectory(Doc)
endif()

if(NOT NO_TOOLS)
	add_subdirectory(NeoPublisher)
	add_subdirectory(tools)
endif()

if(UNIX AND NOT APPLE)
	set(CPACK_GENERATOR "TGZ")
	set(CPACK_STRIP_FILES ON)
endif()

set(CPACK_PACKAGE_NAME "${NEO_PACKAGE}")
set(CPACK_PACKAGE_VENDOR "${NEO_VENDOR}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Neo game engine SDK")
#set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/README.md")
#set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
#set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")
#set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/Data/neo-icon.png")
set(CPACK_PACKAGE_VERSION_MAJOR "${NEO_MAJOR_VERSION}")
set(CPACK_PACKAGE_VERSION_MINOR "${NEO_MINOR_VERSION}")
set(CPACK_PACKAGE_VERSION_PATCH "${NEO_PATCH_VERSION}")
set(CPACK_PACKAGE_CONTACT "Neo Project https://github.com/orgs/NeoGameEngineProject")

## Set install path
#set(CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX}/Neo)

include(CPack)

