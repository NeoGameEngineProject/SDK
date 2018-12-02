file(GLOB_RECURSE HEADERS include/*.h)
file(GLOB_RECURSE SOURCES src/*.c*)

ExternalProject_Add(luajit-build
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/luajit-external
	CONFIGURE_COMMAND ""
	INSTALL_COMMAND ""
	BUILD_COMMAND ${MAKE_EXE}
	BUILD_IN_SOURCE ON
	GIT_REPOSITORY http://luajit.org/git/luajit-2.0.git
)

add_library(luajit STATIC IMPORTED)
set_target_properties(luajit PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/luajit-external/src/libluajit.a)
add_dependencies(luajit luajit-build)

add_library(NeoScript ${SOURCES} ${HEADERS})
target_include_directories(NeoScript PUBLIC include luajit-external/src)

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
target_include_directories(NeoLua PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/luajit-external/src include)

target_include_directories(NeoScript PRIVATE ${CMAKE_SWIG_OUTDIR})
target_link_libraries(NeoScript PUBLIC NeoLua NeoCore NeoEngine luajit)

add_executable(NeoLuaPlayer NeoLua/main.cpp)
target_link_libraries(NeoLuaPlayer PUBLIC NeoScript)

if(NOT NO_TESTS)
	file(GLOB TEST_SOURCES tests/*.c*)
	add_executable(NeoScriptTest ${TEST_SOURCES})
	target_link_libraries(NeoScriptTest ${GTEST_LIBRARIES} NeoScript NeoCore)
endif()
