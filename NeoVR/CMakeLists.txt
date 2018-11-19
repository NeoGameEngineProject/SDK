file(GLOB_RECURSE HEADERS include/*.h)
file(GLOB_RECURSE SOURCES src/*.c*)

include_directories(openvr-external/headers openvr-external/src/vrcommon openvr-external/src)

set(USE_LIBCXX OFF CACHE BOOL "" FORCE)
add_subdirectory(openvr-external)

add_library(NeoVR ${SOURCES} ${HEADERS})
target_link_libraries(NeoVR PUBLIC NeoEngine openvr_api)
target_include_directories(NeoVR PUBLIC include openvr-external/headers openvr-external/src/vrcommon openvr-external/src)
