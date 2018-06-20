file(GLOB SOURCES src/*.c*)
file(GLOB INCLUDES include/*.h)

add_subdirectory(bgfx-cmake EXCLUDE_FROM_ALL)

add_library(renderer ${SOURCES} ${INCLUDES})

target_link_libraries(renderer PUBLIC NeoCore NeoEngine bgfx)
target_include_directories(platform PUBLIC include ../../include)
