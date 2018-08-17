file(GLOB SOURCES src/*.c*)
file(GLOB INCLUDES include/*.h)
file(GLOB_RECURSE SHADERS shaders/*.sc)

## OpenGL
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

## NanoVG
file(GLOB NANOVG_SOURCES nanovg-external/src/*.c*)

add_definitions(-DFONS_USE_FREETYPE)
add_library(nanovg ${NANOVG_SOURCES})

target_include_directories(nanovg PUBLIC
		nanovg-external/src/
		${FREETYPE_INCLUDE_DIRS})

target_link_libraries(nanovg PUBLIC ${FREETYPE_LIBRARIES})
target_include_directories(nanovg PUBLIC ${FREETYPE_INCLUDE_DIRS})

## GLEW
set(BUILD_UTILS CACHE "" OFF FORCE)
add_subdirectory(glew-external/build/cmake EXCLUDE_FROM_ALL)

preprocess_files(SOURCES ${SHADERS}
					INCLUDE_DIRECTORIES "")

add_custom_target(build-shaders SOURCES ${PROCESSED_SHADERS} COMMENT "Building shaders")

add_platform_module(
	SOURCES ${SOURCES} ${INCLUDES}
			${CMAKE_CURRENT_SOURCE_DIR}/../opengl-common/Common.cpp
			${CMAKE_CURRENT_SOURCE_DIR}/../opengl-common/Common.h
	INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/../../include ${INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../opengl-common
	LIBRARIES nanovg glew_s)
	
