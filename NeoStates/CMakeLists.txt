file(GLOB_RECURSE HEADERS include/*.h)
file(GLOB_RECURSE SOURCES src/*.c*)

add_library(NeoStates ${SOURCES} ${HEADERS})
target_link_libraries(NeoStates PUBLIC NeoEngine nanovg)
target_include_directories(NeoStates PUBLIC include)
