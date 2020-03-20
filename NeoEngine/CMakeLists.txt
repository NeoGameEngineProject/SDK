## Find source and headers
file(GLOB_RECURSE HEADERS include/*.h)
file(GLOB_RECURSE SOURCES src/*.c*)

include(AddPlatformModule)

set(CMAKE_SHARED_LIBRARY_PREFIX "")
add_subdirectory(platform)

get_property(NEO_PLATFORM_LIBS GLOBAL PROPERTY NEO_PLATFORM_LIBS_PROP)
get_property(NEO_PLATFORM_SRCS GLOBAL PROPERTY NEO_PLATFORM_SRCS_PROP)
get_property(NEO_PLATFORM_INCS GLOBAL PROPERTY NEO_PLATFORM_INCS_PROP)

if(NEO_BUILD_SHARED_LIBS)
	add_library(NeoEngine SHARED ${SOURCES} ${HEADERS} ${NEO_PLATFORM_SRCS})
else()
	add_library(NeoEngine STATIC ${SOURCES} ${HEADERS} ${NEO_PLATFORM_SRCS})
endif()

if(NOT BUILD_SHARED_LIBS)
	target_compile_definitions(NeoEngine PUBLIC -DNEO_ENGINE_STATIC)
endif()

target_compile_definitions(NeoEngine PRIVATE -DNEO_ENGINE_DLL)
target_include_directories(NeoEngine PUBLIC
	${NEO_PLATFORM_INCS}
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_BINARY_DIR}
	include
	PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/rapidjson)

if(NOT WII)
	set(DL_LIBS ${CMAKE_DL_LIBS})
endif()

target_link_libraries(NeoEngine PUBLIC NeoCore ${DL_LIBS} ${NEO_PLATFORM_LIBS} flatbuffers)

if(NOT NO_TESTS)
	file(GLOB TEST_SOURCES tests/*.c*)
	add_executable(NeoEngineTest ${TEST_SOURCES})
	target_link_libraries(NeoEngineTest ${GTEST_LIBRARIES} NeoEngine)

	if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" AND ENABLE_FUZZ)
		add_executable(NeoEngineSerializeFuzz fuzz/serialize.cpp)

		set(FUZZ_OPTS -fsanitize-coverage=indirect-calls,trace-cmp,trace-div,trace-gep -gline-tables-only)
		target_compile_options(NeoEngineSerializeFuzz PRIVATE ${FUZZ_OPTS} -fsanitize=fuzzer,address)
		target_link_libraries(NeoEngineSerializeFuzz NeoEngine ${FUZZ_OPTS} -fsanitize=fuzzer,address)

		target_compile_options(NeoEngine PRIVATE ${FUZZ_OPTS} -fsanitize=fuzzer,address)
		target_link_options(NeoEngine PRIVATE ${FUZZ_OPTS} -fsanitize=fuzzer,address)
		
		target_compile_options(NeoCore PRIVATE ${FUZZ_OPTS} -fsanitize=fuzzer,address)
		target_link_options(NeoCore PRIVATE ${FUZZ_OPTS} -fsanitize=fuzzer,address)
		
	endif()
endif()

get_link_libraries(DEPS NeoEngine)
fix_install_includes(NeoEngine)

install(TARGETS NeoEngine ${DEPS}
		EXPORT FindNeoEngine
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib)
	
install(EXPORT FindNeoEngine DESTINATION ${NEO_CMAKE_EXPORT})
install(DIRECTORY include/ DESTINATION include/)

get_target_property(INCS NeoEngine INCLUDE_DIRECTORIES)

if(NOT CMAKE_CROSSCOMPILING)
	add_custom_target(neoengine-schema
		COMMAND $<TARGET_FILE:flatc> --binary --cpp -o ${CMAKE_CURRENT_BINARY_DIR} --cpp-std "c++11" ${CMAKE_CURRENT_SOURCE_DIR}/schemas/NeoEngine.fbs
		SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/schemas/NeoEngine.fbs
		DEPENDS flatc
	)

	add_dependencies(NeoEngine neoengine-schema)
endif()

#foreach(INC ${INCS})
#	install(DIRECTORY ${INC} DESTINATION include/)
#endforeach()
