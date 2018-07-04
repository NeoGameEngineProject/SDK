file(GLOB SOURCES src/*.c*)
file(GLOB INCLUDES include/*.h)
file(GLOB_RECURSE SHADERS shaders/*.sc)

set(BGFX_AMALGAMATED CACHE "" ON FORCE)
set(BX_AMALGAMATED CACHE "" ON FORCE)
set(OpenGL_GL_PREFERENCE GLVND)
add_subdirectory(bgfx-cmake-external EXCLUDE_FROM_ALL)

## NanoVG used by Neo2D
find_package(Freetype REQUIRED)
file(GLOB NANOVG_SOURCES bgfx-cmake-external/bgfx/examples/common/nanovg/*.c*)
add_definitions(-DFONS_USE_FREETYPE)

## Depend on shaders, despite them not being used here. It is used as a hook for NeoEngine
## to build the shaders.
add_library(nanovg ${NANOVG_SOURCES} ${SHADERS})

target_include_directories(nanovg PUBLIC 
	bgfx-cmake-external/bgfx/3rdparty/
	bgfx-cmake-external/bgfx/examples/common/nanovg
	${FREETYPE_INCLUDE_DIRS})
	
target_link_libraries(nanovg PUBLIC bgfx bx bimg ${FREETYPE_LIBRARIES})
	
add_shader(shaders/base/base_vs.sc VERTEX OUTPUT ${CMAKE_BINARY_DIR}/bin/assets PLATFORMS glsl GLSL_VERSION 130)
add_shader(shaders/base/base_fs.sc FRAGMENT OUTPUT ${CMAKE_BINARY_DIR}/bin/assets PLATFORMS glsl GLSL_VERSION 130)

add_shader(shaders/def_phong_vs.sc VERTEX OUTPUT ${CMAKE_BINARY_DIR}/bin/assets PLATFORMS glsl GLSL_VERSION 130)
add_shader(shaders/def_phong_fs.sc FRAGMENT OUTPUT ${CMAKE_BINARY_DIR}/bin/assets PLATFORMS glsl GLSL_VERSION 130)

add_platform_module(
	SOURCES ${SOURCES} ${INCLUDES}
	INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/../../include
	LIBRARIES bgfx nanovg)
	
