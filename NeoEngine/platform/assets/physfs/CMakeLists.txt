file(GLOB SOURCES src/*.c*)
file(GLOB INCLUDES include/*.h)

set(PHYSFS_BUILD_SHARED CACHE "" OFF FORCE)
set(PHYSFS_BUILD_TEST CACHE "" OFF FORCE)
add_subdirectory(physfs-external EXCLUDE_FROM_ALL)

add_library(vfs ${SOURCES} ${INCLUDES})
target_include_directories(vfs PUBLIC include physfs-external/src)
target_link_libraries(vfs PUBLIC physfs-static NeoCore)
