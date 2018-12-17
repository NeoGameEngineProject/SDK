file(GLOB_RECURSE HEADERS include/*.h)
file(GLOB_RECURSE SOURCES src/*.c*)

include_directories(openvr-external/headers openvr-external/src/vrcommon openvr-external/src)

set(USE_LIBCXX OFF CACHE BOOL "" FORCE)
add_subdirectory(openvr-external)

add_library(NeoVR ${SOURCES} ${HEADERS})
target_link_libraries(NeoVR PUBLIC NeoEngine openvr_api)
target_include_directories(NeoVR PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/openvr-external/headers>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/openvr-external/src/vrcommon>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/openvr-external/src>
	$<INSTALL_INTERFACE:include>)

install(TARGETS NeoVR openvr_api NeoCore
		EXPORT NeoVR
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib)
	
install(EXPORT NeoVR DESTINATION ${NEO_CMAKE_EXPORT})
