file(GLOB SOURCES src/*.c*)
file(GLOB SHADERS shaders/*.sc)
file(GLOB INCLUDES include/*.h)

add_subdirectory(bgfx-cmake EXCLUDE_FROM_ALL)

add_library(renderer ${SOURCES} ${INCLUDES} ${SHADERS})

target_link_libraries(renderer PUBLIC NeoCore NeoEngine bgfx)
target_include_directories(renderer PUBLIC include ../../include)

add_shader(shaders/base_vs.sc VERTEX OUTPUT ${CMAKE_BINARY_DIR}/bin/assets PLATFORMS glsl GLSL_VERSION 130)
add_shader(shaders/base_fs.sc FRAGMENT OUTPUT ${CMAKE_BINARY_DIR}/bin/assets PLATFORMS glsl GLSL_VERSION 130)
