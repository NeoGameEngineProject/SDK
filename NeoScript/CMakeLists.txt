file(GLOB_RECURSE HEADERS include/*.h)
file(GLOB_RECURSE SOURCES src/*.c*)

find_program(MAKE_EXE make)
ExternalProject_Add(luajit-build
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/luajit-external
	CONFIGURE_COMMAND ""
	INSTALL_COMMAND ""
	BUILD_COMMAND sh -e ${CMAKE_CURRENT_SOURCE_DIR}/build-luajit-linux-shared.sh "${CMAKE_C_COMPILER}"
	BUILD_IN_SOURCE ON
	GIT_REPOSITORY http://luajit.org/git/luajit-2.0.git
)

if(CMAKE_POSITION_INDEPENDENT_CODE)
	add_library(luajit SHARED IMPORTED GLOBAL)
	set_target_properties(luajit PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/luajit-external/src/libluajit.so)
else()
	add_library(luajit STATIC IMPORTED GLOBAL)
	set_target_properties(luajit PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/luajit-external/src/libluajit.a)
endif()

add_dependencies(luajit luajit-build)

add_library(NeoScript ${SOURCES} ${HEADERS})
target_include_directories(NeoScript PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/luajit-external/src>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include>)

find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})

set_property(SOURCE swig/NeoEngineLua.i PROPERTY CPLUSPLUS ON)

swig_add_library(NeoLua
	TYPE STATIC 
	LANGUAGE lua 
	SOURCES swig/NeoEngineLua.i)

get_property(NEO_ENGINE_INCLUDES TARGET NeoEngine PROPERTY INCLUDE_DIRECTORIES)
set_property(TARGET NeoLua PROPERTY SWIG_INCLUDE_DIRECTORIES
	${CMAKE_CURRENT_SOURCE_DIR}/../NeoEngine/include
	${CMAKE_CURRENT_SOURCE_DIR}/../NeoCore/include
	${NEO_ENGINE_INCLUDES})

swig_link_libraries(NeoLua PUBLIC NeoEngine)
target_include_directories(NeoLua PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/luajit-external/src>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include>)

target_include_directories(NeoScript PRIVATE ${CMAKE_SWIG_OUTDIR})
target_link_libraries(NeoScript PUBLIC NeoLua NeoEngine luajit)
target_link_directories(NeoScript PUBLIC $<INSTALL_INTERFACE:lib>)

add_executable(NeoLuaPlayer NeoLua/main.cpp)
target_link_libraries(NeoLuaPlayer PUBLIC NeoScript)

if(NOT NO_TESTS)
	file(GLOB TEST_SOURCES tests/*.c*)
	add_executable(NeoScriptTest ${TEST_SOURCES})
	target_link_libraries(NeoScriptTest ${GTEST_LIBRARIES} NeoScript NeoCore)
endif()

install(FILES $<TARGET_FILE:luajit> DESTINATION lib)
install(TARGETS NeoLuaPlayer NeoLua NeoScript
		EXPORT FindNeoScript
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib)
	
install(EXPORT FindNeoScript DESTINATION ${NEO_CMAKE_EXPORT})
