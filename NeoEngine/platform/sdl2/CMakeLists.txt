file(GLOB SOURCES src/*.c*)
file(GLOB INCLUDES include/*.h)

find_package(SDL2 REQUIRED)

set(PNG_SHARED CACHE OFF "" FORCE)
set(PNG_TESTS CACHE OFF "" FORCE)

add_library(platform ${SOURCES} ${INCLUDES})

target_link_libraries(platform PUBLIC NeoCore NeoEngine ${SDL2_LIBRARY} renderer levelload textureload)
target_include_directories(platform PUBLIC include ../../include ${SDL2_INCLUDE_DIR})
