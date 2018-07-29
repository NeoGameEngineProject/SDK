file(GLOB_RECURSE HEADERS include/*.h)
file(GLOB_RECURSE SOURCES src/*.c*)

add_definitions(-DLITEHTML_UTF8)
add_subdirectory(litehtml-external)

if(CMAKE_COMPILER_IS_GNUCC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive")
endif()

add_library(NeoHTML ${SOURCES} ${HEADERS} include/master.css.inc)
target_link_libraries(NeoHTML PUBLIC litehtml nanovg NeoEngine)
target_include_directories(NeoHTML PUBLIC include litehtml-external/include)

if(NOT WIN32)
	find_program(XXD_COMMAND xxd)
	add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/include/master.css.inc
					   COMMAND cat ${CMAKE_CURRENT_SOURCE_DIR}/litehtml-external/include/master.css | xxd -i > ${CMAKE_CURRENT_SOURCE_DIR}/include/master.css.inc)
else()
	target_compile_definitions(NeoHTML PUBLIC -DLITEHTML_UTF8)
	add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/include/master.css.inc
					   COMMAND PowerShell -Command "Get-Content ${CMAKE_CURRENT_SOURCE_DIR}/include/master.css.inc > ${CMAKE_CURRENT_SOURCE_DIR}/litehtml-external/include/master.css")
endif()

set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/include/master.css.inc PROPERTIES GENERATED TRUE)
