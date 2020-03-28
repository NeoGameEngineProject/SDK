file(GLOB_RECURSE HEADERS include/*.h)
file(GLOB_RECURSE SOURCES src/*.c*)

add_subdirectory(enet-external)

add_library(NeoNet ${SOURCES} ${HEADERS})
target_link_libraries(NeoNet PUBLIC NeoEngine enet)

target_include_directories(NeoNet PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
	$<INSTALL_INTERFACE:include>)

fix_install_includes(NeoNet)

if(NOT NO_TESTS)
	file(GLOB TEST_SOURCES tests/*.c*)
	add_executable(NeoNetTest ${TEST_SOURCES})
	target_link_libraries(NeoNetTest ${GTEST_LIBRARIES} NeoNet)
endif()

install(TARGETS NeoNet enet
		EXPORT FindNeoNet
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib)

install(EXPORT FindNeoNet DESTINATION ${NEO_CMAKE_EXPORT})
install(DIRECTORY include/ DESTINATION include)
