file(GLOB SOURCES src/*.c*)
file(GLOB INCLUDES include/*.h)

add_subdirectory(libzip-external EXCLUDE_FROM_ALL)

add_library(vfs ${SOURCES} ${INCLUDES})
target_include_directories(vfs PUBLIC include libzip-external/lib)
target_link_libraries(vfs PUBLIC zip NeoCore)
