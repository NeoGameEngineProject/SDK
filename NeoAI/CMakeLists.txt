file(GLOB_RECURSE HEADERS include/*.h)
file(GLOB_RECURSE SOURCES src/*.c*)

set(RECASTNAVIGATION_DEMO CACHE "" OFF FORCE)
set(RECASTNAVIGATION_TESTS CACHE "" OFF FORCE)
set(RECASTNAVIGATION_EXAMPLES CACHE "" OFF FORCE)
add_subdirectory(recast-external EXCLUDE_FROM_ALL)

add_library(NeoAI ${SOURCES} ${HEADERS})
target_link_libraries(NeoAI PUBLIC NeoEngine Recast Detour)
target_include_directories(NeoAI PUBLIC 
	include 
	recast-external/Recast/Include
	recast-external/Detour/Include)

