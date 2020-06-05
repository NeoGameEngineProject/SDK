file(GLOB SOURCES src/*.c*)
file(GLOB INCLUDES include/*.h)
file(GLOB_RECURSE SHADERS shaders/*)

## OpenGL
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

## NanoVG
file(GLOB NANOVG_SOURCES nanovg-external/src/*.c*)
file(GLOB NANOVG_HEADERS nanovg-external/src/*.h*)

add_definitions(-DFONS_USE_FREETYPE)
add_library(nanovg ${NANOVG_SOURCES})

# Or else nanovg does not export anything
set_target_properties(nanovg PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)

target_include_directories(nanovg PUBLIC
	${FREETYPE_INCLUDE_DIRS}
	nanovg-external/src/)

target_link_libraries(nanovg PUBLIC ${FREETYPE_LIBRARIES})

fix_install_includes(nanovg)
install(FILES ${NANOVG_HEADERS} DESTINATION include)

# imgui, yeah!
file(GLOB IMGUI_SOURCES imgui-external/*.c*)
file(GLOB IMGUIZMO_SOURCES ImGuizmo/*.c*)

file(GLOB IMGUI_HEADERS imgui-external/*.h)
file(GLOB IMGUIZMO_HEADERS ImGuizmo/*.h)

add_library(imgui STATIC ${IMGUI_SOURCES} ${IMGUIZMO_SOURCES})
target_include_directories(imgui PUBLIC imgui-external ImGuizmo glew-external/include)
fix_install_includes(imgui)

install(FILES ${IMGUI_HEADERS} ${IMGUIZMO_HEADERS} DESTINATION include)

## GLEW
set(BUILD_UTILS CACHE "" OFF FORCE)
add_subdirectory(glew-external/build/cmake)

if(NOT WIN32 AND NOT BUILD_SHARED_LIBS)
	set(GLEW_LIBRARIES glew_s)
	fix_install_includes(glew_s)
else()
	set(GLEW_LIBRARIES glew)
	fix_install_includes(glew)
endif()

make_directory(${CMAKE_BINARY_DIR}/bin/assets/materials/builtin/)
preprocess_files(SOURCES ${SHADERS} ${CMAKE_CURRENT_SOURCE_DIR}/shaders/ShaderLight.h INCLUDE_DIRECTORIES "")
add_custom_target(build-shaders SOURCES ${PROCESSED_SHADERS} COMMENT "Building shaders")

add_platform_module(
	SOURCES ${SOURCES} ${INCLUDES}
			${CMAKE_CURRENT_SOURCE_DIR}/../opengl-common/Common.cpp
			${CMAKE_CURRENT_SOURCE_DIR}/../opengl-common/Common.h
			${CMAKE_CURRENT_SOURCE_DIR}/../opengl-common/Preprocessor.cpp
			${CMAKE_CURRENT_SOURCE_DIR}/../opengl-common/Preprocessor.h
	INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/include 
			${INCLUDE_DIR} 
			${CMAKE_CURRENT_SOURCE_DIR}/../opengl-common 
			${CMAKE_CURRENT_SOURCE_DIR}/glew-external/include
	LIBRARIES nanovg imgui ${GLEW_LIBRARIES})

install(FILES ../opengl-common/Common.h DESTINATION include)
install(DIRECTORY include/ DESTINATION include)
#install(DIRECTORY materials DESTINATION bin/assets/)
install(DIRECTORY ${CMAKE_BINARY_DIR}/bin/assets DESTINATION bin/)

include(CopyFiles)

copy_files("materials/*" "${CMAKE_BINARY_DIR}/bin/assets/" "Materials")
# copy_files("materials/builtin/*" "${CMAKE_BINARY_DIR}/bin/assets/" "BuiltinShaders")

#install(TARGETS nanovg
#		EXPORT FindNanovg
#		RUNTIME DESTINATION bin
#		LIBRARY DESTINATION lib
#		ARCHIVE DESTINATION lib)
#	
#install(EXPORT FindNanovg DESTINATION ${NEO_CMAKE_EXPORT})

#install(TARGETS imgui
#		EXPORT FindImgui
#		RUNTIME DESTINATION bin
#		LIBRARY DESTINATION lib
#		ARCHIVE DESTINATION lib)
#	
#install(EXPORT FindImgui DESTINATION ${NEO_CMAKE_EXPORT})

#install(TARGETS ${GLEW_LIBRARIES}
#		EXPORT FindGLEW
#		RUNTIME DESTINATION bin
#		LIBRARY DESTINATION lib
#		ARCHIVE DESTINATION lib)
	
#install(EXPORT FindGLEW DESTINATION ${NEO_CMAKE_EXPORT})


