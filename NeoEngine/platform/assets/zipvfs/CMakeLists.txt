file(GLOB SOURCES src/*.c*)
file(GLOB INCLUDES include/*.h)

add_subdirectory(libzip-external EXCLUDE_FROM_ALL)

add_library(zipvfs ${SOURCES} ${INCLUDES})
target_include_directories(zipvfs PUBLIC include libzip-external/lib)
target_link_libraries(zipvfs PUBLIC zip NeoCore)
