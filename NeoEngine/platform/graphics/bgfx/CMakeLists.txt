file(GLOB SOURCES src/*.c*)
file(GLOB_RECURSE SHADERS shaders/*.sc)
file(GLOB INCLUDES include/*.h)

add_subdirectory(bgfx-cmake-external EXCLUDE_FROM_ALL)

## NanoVG used by Neo2D
find_package(Freetype REQUIRED)
file(GLOB NANOVG_SOURCES bgfx-cmake-external/bgfx/examples/common/nanovg/*.c*)
add_definitions(-DFONS_USE_FREETYPE)
add_library(nanovg ${NANOVG_SOURCES})

target_include_directories(nanovg PUBLIC 
	bgfx-cmake-external/bgfx/3rdparty/
	bgfx-cmake-external/bgfx/examples/common/nanovg
	${FREETYPE_INCLUDE_DIRS})
	
target_link_libraries(nanovg PUBLIC bgfx bx bimg ${FREETYPE_LIBRARIES})

add_library(renderer ${SOURCES} ${INCLUDES} ${SHADERS})

target_link_libraries(renderer PUBLIC NeoCore NeoEngine bgfx nanovg)
target_include_directories(renderer PUBLIC include ../../include)

add_shader(shaders/base/base_vs.sc VERTEX OUTPUT ${CMAKE_BINARY_DIR}/bin/assets PLATFORMS glsl GLSL_VERSION 130)
add_shader(shaders/base/base_fs.sc FRAGMENT OUTPUT ${CMAKE_BINARY_DIR}/bin/assets PLATFORMS glsl GLSL_VERSION 130)

add_shader(shaders/def_phong_vs.sc VERTEX OUTPUT ${CMAKE_BINARY_DIR}/bin/assets PLATFORMS glsl GLSL_VERSION 130)
add_shader(shaders/def_phong_fs.sc FRAGMENT OUTPUT ${CMAKE_BINARY_DIR}/bin/assets PLATFORMS glsl GLSL_VERSION 130)
