project(Neo)
cmake_minimum_required(VERSION 3.10)

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

include(CopyFiles)
include(ExternalProject)

set(CMAKE_CXX_STANDARD 14)
set(BUILD_SHARED_LIBS OFF)
set(SANITIZER "address" CACHE STRING "The sanitizer to use")

if(NEO_BUILD_SHARED_LIBS)
	set(CMAKE_POSITION_INDEPENDENT_CODE ON)
	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-rpath .")
endif()

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
include(PreProcessor)

if(NOT NO_TESTS)
	find_package(GTest)
	if(NOT GTEST_FOUND)
		set(NO_TESTS TRUE)
	endif()
endif()

if(ENABLE_SANITIZERS)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=${SANITIZER} -fsanitize=undefined")
endif()

if(ENABLE_OPENGL_RENDERER)
	add_definitions(-DNEO_OPENGL=1)
endif()

add_subdirectory(NeoCore)
add_subdirectory(NeoEngine)
add_subdirectory(NeoHTML)
add_subdirectory(NeoStates)
add_subdirectory(NeoVR)
add_subdirectory(NeoScript)

if(DISABLE_MULTITHREAD)
	target_compile_definitions(NeoEngine PUBLIC -DNEO_SINGLE_THREAD=1)
endif()

# add_subdirectory(NeoAI)
# add_subdirectory(Neo2D)

if(ENABLE_SANITIZERS)
	target_compile_options(NeoEngine PUBLIC -fsanitize=${SANITIZER} -fsanitize=undefined -fno-omit-frame-pointer)
	target_compile_options(NeoCore PUBLIC -fsanitize=${SANITIZER} -fsanitize=undefined -fno-omit-frame-pointer)
endif()

if(NOT NO_TESTGAME)
	add_subdirectory(testgame)
endif()

if(NOT NO_DOCS)
	add_subdirectory(Doc)
endif()

if(UNIX AND NOT APPLE)
	set(CPACK_GENERATOR "STGZ;TGZ;DEB;RPM;")
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
set(CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX}/Neo)

include(CPack)

