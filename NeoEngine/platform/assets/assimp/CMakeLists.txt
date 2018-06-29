file(GLOB SOURCES src/*.c*)
file(GLOB INCLUDES include/*.h)

set(ASSIMP_NO_EXPORT CACHE ON "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS CACHE OFF "" FORCE)
set(ASSIMP_BUILD_TESTS CACHE OFF "" FORCE)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT CACHE OFF "" FORCE)

set(ASSIMP_BUILD_COLLADA_IMPORTER ON)

add_subdirectory(assimp EXCLUDE_FROM_ALL)
add_library(levelload ${SOURCES} ${INCLUDES})

target_link_libraries(levelload PUBLIC NeoCore NeoEngine assimp)
target_include_directories(levelload PUBLIC include)
