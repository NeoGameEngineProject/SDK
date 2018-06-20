file(GLOB SOURCES src/*.c*)
file(GLOB INCLUDES include/*.h)

find_package(SDL2 REQUIRED)

add_library(platform ${SOURCES} ${INCLUDES})

target_link_libraries(platform PUBLIC NeoCore NeoEngine ${SDL2_LIBRARY} renderer levelload)
target_include_directories(platform PUBLIC include ../../include ${SDL2_INCLUDE_DIR})
