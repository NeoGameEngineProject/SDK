file(GLOB_RECURSE HEADERS include/*.h)
file(GLOB_RECURSE SOURCES src/*.c*)

add_library(NeoStates ${SOURCES} ${HEADERS})
target_link_libraries(NeoStates PUBLIC NeoEngine nanovg)
target_include_directories(NeoStates PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:include>)

install(TARGETS NeoStates
		EXPORT NeoStates
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib)
	
install(EXPORT NeoStates DESTINATION ${NEO_CMAKE_EXPORT})
