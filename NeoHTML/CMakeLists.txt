file(GLOB_RECURSE HEADERS include/*.h)
file(GLOB_RECURSE SOURCES src/*.c*)

if(WIN32)
	add_definitions(-DLITEHTML_UTF8)
endif()

if(WIN32)
	set(BUILD_SHARED_LIBS FALSE)
endif()

add_subdirectory(litehtml-external EXCLUDE_FROM_ALL)

if(WIN32)
	set(BUILD_SHARED_LIBS TRUE)
endif()

set_property(TARGET gumbo PROPERTY WINDOWS_EXPORT_ALL_SYMBOLS ON)
set_property(TARGET litehtml PROPERTY WINDOWS_EXPORT_ALL_SYMBOLS ON)

if(CMAKE_COMPILER_IS_GNUCC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive")
endif()

add_library(NeoHTML ${SOURCES} ${HEADERS} include/master.css.inc)
target_link_libraries(NeoHTML PUBLIC litehtml nanovg NeoEngine)
target_include_directories(NeoHTML PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/litehtml-external/include
	$<INSTALL_INTERFACE:include>)

if(NOT WIN32 OR CMAKE_CROSSCOMPILING)
	find_program(XXD_COMMAND xxd)
	add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/include/master.css.inc
					   COMMAND cat ${CMAKE_CURRENT_SOURCE_DIR}/litehtml-external/include/master.css | xxd -i > ${CMAKE_CURRENT_SOURCE_DIR}/include/master.css.inc)
else()
	target_compile_definitions(NeoHTML PUBLIC -DLITEHTML_UTF8)
	add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/include/master.css.inc
					   COMMAND PowerShell -Command "Get-Content ${CMAKE_CURRENT_SOURCE_DIR}/include/master.css.inc > ${CMAKE_CURRENT_SOURCE_DIR}/litehtml-external/include/master.css")
endif()

set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/include/master.css.inc PROPERTIES GENERATED TRUE)

fix_install_includes(NeoHTML)

if(WIN32)
	install(TARGETS NeoHTML
			EXPORT FindNeoHTML
			RUNTIME DESTINATION bin
			LIBRARY DESTINATION lib
			ARCHIVE DESTINATION lib)

	install(DIRECTORY litehtml-external/include/ DESTINATION include/litehtml)
else()
	install(TARGETS NeoHTML
			EXPORT FindNeoHTML
			RUNTIME DESTINATION bin
			LIBRARY DESTINATION lib
			ARCHIVE DESTINATION lib)
endif()

install(EXPORT FindNeoHTML DESTINATION ${NEO_CMAKE_EXPORT})
install(DIRECTORY include/ DESTINATION include)
