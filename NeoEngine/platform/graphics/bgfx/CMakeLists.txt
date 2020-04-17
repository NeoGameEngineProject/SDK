#file(GLOB SOURCES src/*.c*)
#file(GLOB INCLUDES include/*.h)
file(GLOB_RECURSE SHADERS shaders/*.sc)

set(USE_DEFERRED OFF)
set(SOURCES 
	${CMAKE_CURRENT_SOURCE_DIR}/src/PlatformStaticRenderBehavior.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/PlatformSkyboxBehavior.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/PlatformParticleSystemBehavior.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Common.cpp
)
if(USE_DEFERRED AND NOT EMSCRIPTEN)
	set(SOURCES 
		${SOURCES}
		${CMAKE_CURRENT_SOURCE_DIR}/src/deferred/PlatformRenderer.cpp)
		
	set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/deferred)
	add_definitions(-DBGFX_DEFERRED=1)
else()
	set(SOURCES 
		${SOURCES}
		${CMAKE_CURRENT_SOURCE_DIR}/src/forward/PlatformRenderer.cpp)
	
	set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/forward)
	add_definitions(-DBGFX_FORWARD=1)
endif()

set(BGFX_AMALGAMATED CACHE "" ON FORCE)
set(BX_AMALGAMATED CACHE "" ON FORCE)
set(OpenGL_GL_PREFERENCE GLVND)
add_subdirectory(bgfx-cmake-external EXCLUDE_FROM_ALL)

## NanoVG used by Neo2D
find_package(Freetype)
if(NOT FREETYPE_FOUND)
	file(GLOB_RECURSE FREETYPE2_SRC freetype2-external/src/*.c*)
	set(FREETYPE_LIBRARIES freetype)
	set(FREETYPE_INCLUDE_DIRS freetype2-external/include)
	set(SKIP_INSTALL_ALL ON)

	add_subdirectory(freetype2-external)
endif()

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

## ImGui, yeah!
file(GLOB IMGUI_SOURCES bgfx-cmake-external/bgfx/examples/common/imgui/*.c*)

add_library(imgui STATIC
	${IMGUI_SOURCES}
	bgfx-cmake-external/bgfx/3rdparty/dear-imgui/imgui.cpp
	bgfx-cmake-external/bgfx/3rdparty/dear-imgui/imgui_draw.cpp
	bgfx-cmake-external/bgfx/3rdparty/dear-imgui/imgui_widgets.cpp
)

target_include_directories(imgui PUBLIC 
	bgfx-cmake-external/bgfx/3rdparty/
	# bgfx-cmake-external/bgfx/3rdparty/dear-imgui
	bgfx-cmake-external/bgfx/examples/common/imgui)

target_link_libraries(imgui PUBLIC bgfx bx bimg)

if(NOT EMSCRIPTEN)
	#add_shader(shaders/base/base_vs.sc VERTEX OUTPUT ${CMAKE_BINARY_DIR}/bin/assets PLATFORMS glsl asm.js GLSL_VERSION 130)
	#add_shader(shaders/base/base_fs.sc FRAGMENT OUTPUT ${CMAKE_BINARY_DIR}/bin/assets PLATFORMS glsl asm.js GLSL_VERSION 130)

	#add_shader(shaders/def_phong_vs.sc VERTEX OUTPUT ${CMAKE_BINARY_DIR}/bin/assets PLATFORMS glsl asm.js  GLSL_VERSION 130)
	#add_shader(shaders/def_phong_fs.sc FRAGMENT OUTPUT ${CMAKE_BINARY_DIR}/bin/assets PLATFORMS glsl asm.js GLSL_VERSION 130)
	
	if(USE_DEFERRED)
		add_shader(shaders/deferred/geometry/base_vs.sc VERTEX OUTPUT ${CMAKE_BINARY_DIR}/bin/assets PLATFORMS glsl asm.js GLSL_VERSION 130)
		add_shader(shaders/deferred/geometry/base_fs.sc FRAGMENT OUTPUT ${CMAKE_BINARY_DIR}/bin/assets PLATFORMS glsl asm.js GLSL_VERSION 130)

		add_shader(shaders/deferred/def_phong_vs.sc VERTEX OUTPUT ${CMAKE_BINARY_DIR}/bin/assets PLATFORMS glsl GLSL_VERSION 130)
		add_shader(shaders/deferred/def_phong_fs.sc FRAGMENT OUTPUT ${CMAKE_BINARY_DIR}/bin/assets PLATFORMS glsl GLSL_VERSION 130)
	else()
		add_shader(shaders/forward/geometry/phong_vs.sc VERTEX OUTPUT ${CMAKE_BINARY_DIR}/bin/assets PLATFORMS glsl asm.js GLSL_VERSION 130)
		add_shader(shaders/forward/geometry/phong_fs.sc FRAGMENT OUTPUT ${CMAKE_BINARY_DIR}/bin/assets PLATFORMS glsl asm.js GLSL_VERSION 130)
	endif()
	
	add_custom_target(build-shaders SOURCES ${SHADERS})
endif()

fix_install_includes(nanovg)
fix_install_includes(imgui)

add_platform_module(
	SOURCES ${SOURCES} ${INCLUDES}
	INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/../../include ${INCLUDE_DIR}
	LIBRARIES bgfx nanovg imgui)
	
