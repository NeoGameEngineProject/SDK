file(GLOB_RECURSE HEADERS include/*.h)
file(GLOB_RECURSE SOURCES src/*.c*)

find_program(MAKE_EXE make)

# Linux yay!
if(NOT APPLE AND UNIX)
	ExternalProject_Add(luajit-build
		SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/luajit-external
		CONFIGURE_COMMAND ""
		INSTALL_COMMAND ""
		BUILD_COMMAND sh -e ${CMAKE_CURRENT_SOURCE_DIR}/build-luajit-linux-shared.sh "${CMAKE_C_COMPILER}"
		BUILD_IN_SOURCE ON
		GIT_REPOSITORY http://luajit.org/git/luajit-2.0.git
	)
elseif(APPLE)
	ExternalProject_Add(luajit-build
		SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/luajit-external
		CONFIGURE_COMMAND ""
		INSTALL_COMMAND ""
		BUILD_COMMAND sh -e ${CMAKE_CURRENT_SOURCE_DIR}/build-luajit-macos-shared.sh "cc"# "${CMAKE_C_COMPILER}"
		BUILD_IN_SOURCE ON
		GIT_REPOSITORY http://luajit.org/git/luajit-2.0.git
	)
elseif(WIN32 AND CMAKE_CROSSCOMPILING)
	ExternalProject_Add(luajit-build
		SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/luajit-external
		CONFIGURE_COMMAND ""
		INSTALL_COMMAND ""
		BUILD_COMMAND sh -e ${CMAKE_CURRENT_SOURCE_DIR}/build-luajit-win32-shared.sh "${CMAKE_C_COMPILER}"
		BUILD_IN_SOURCE ON
		GIT_REPOSITORY http://luajit.org/git/luajit-2.0.git
	)

	add_custom_command(TARGET luajit-build POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
			${CMAKE_CURRENT_SOURCE_DIR}/luajit-external/src/lua51.dll 
			${CMAKE_BINARY_DIR}/bin
	)
elseif(WIN32 AND NOT CMAKE_CROSSCOMPILING)
	ExternalProject_Add(luajit-build
		SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/luajit-external
		CONFIGURE_COMMAND ""
		INSTALL_COMMAND ""
		BUILD_COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/build-luajit-win32-msvc.bat "${CMAKE_C_COMPILER}"
		BUILD_IN_SOURCE ON
		GIT_REPOSITORY http://luajit.org/git/luajit-2.0.git
	)

	add_custom_command(TARGET luajit-build POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
			${CMAKE_CURRENT_SOURCE_DIR}/luajit-external/src/lua51.dll 
			${CMAKE_BINARY_DIR}/bin
	)
	
	if(NOT EXISTS ${CMAKE_BINARY_DIR}/swigwin.zip)
		file(DOWNLOAD "https://downloads.sourceforge.net/project/swig/swigwin/swigwin-4.0.1/swigwin-4.0.1.zip" "${CMAKE_BINARY_DIR}/swigwin.zip" SHOW_PROGRESS)
		execute_process(
			COMMAND ${CMAKE_COMMAND} -E tar xzf ${CMAKE_BINARY_DIR}/swigwin.zip
			WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
		)
	endif()
	
	set(SWIG_EXECUTABLE "${CMAKE_BINARY_DIR}/swigwin-4.0.1/swig.exe")
endif()

#if(CMAKE_POSITION_INDEPENDENT_CODE AND NOT APPLE)
#	add_library(luajit SHARED IMPORTED GLOBAL)
#	set_target_properties(luajit PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/luajit-external/src/libluajit.so)
#else()
if(WIN32)
	if(NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
		add_library(luajit STATIC IMPORTED GLOBAL)
		set_target_properties(luajit PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/luajit-external/src/libluajit-5.1.dll.a)
		install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/luajit-external/src/libluajit-5.1.dll.a DESTINATION lib)
		install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/luajit-external/src/libluajit-5.1.dll DESTINATION lib)
	else()
		add_library(luajit UNKNOWN IMPORTED GLOBAL)
			set_target_properties(luajit PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/luajit-external/src/lua51.lib)
			install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/luajit-external/src/lua51.dll DESTINATION bin)
			install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/luajit-external/src/lua51.lib DESTINATION lib)

	endif()
else()
	add_library(luajit STATIC IMPORTED GLOBAL)
	set_target_properties(luajit PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/luajit-external/src/libluajit.a)
endif()
#endif()

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
