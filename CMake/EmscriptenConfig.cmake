
set(SDL2_LIBRARY "-s USE_SDL=2")
set(SDL2_INCLUDE_DIR " ")
set(SDL2_FOUND TRUE)

set(FREETYPE_LIBRARY "freetype")
set(FREETYPE_INCLUDE_DIRS " ")
set(FREETYPE_FOUND TRUE)

set(ZLIB_LIBRARY "zlib")
set(ZLIB_INCLUDE_DIR " ")
set(ZLIB_FOUND TRUE)

set(M_LIBRARY "m")
set(M_INCLUDE_DIR " ")
set(M_FOUND TRUE)

set(WASM 1)
set(WEBGL2 1)

set(ENABLE_OPENGL_RENDERER TRUE)
set(CMAKE_EXECUTABLE_SUFFIX ".html")

add_compile_options(
	-sUSE_SDL=2
	-sUSE_FREETYPE=1
	-sUSE_ZLIB=1
	-sUSE_LIBPNG=1
)

add_link_options(
	-sERROR_ON_UNDEFINED_SYMBOLS=0
	-sLLD_REPORT_UNDEFINED
	-sUSE_SDL=2
	-sUSE_FREETYPE=1
	-sUSE_LIBPNG=1
	-sUSE_ZLIB=1
	-lopenal
	-sUSE_WEBGL2=${WEBGL2}
	-sFULL_ES3 
	-sWASM=${WASM}
	-sALLOW_MEMORY_GROWTH=${WASM}
	-sTOTAL_MEMORY=1GB
	--shell-file ${CMAKE_SOURCE_DIR}/scripts/emscripten_skeleton.html
	--source-map-base ${CMAKE_CURRENT_BINARY_DIR}
)

if("${CMAKE_BUILD_TYPE}" MATCHES "Release")
	add_compile_options(-O3 -flto)
	add_link_options(-O3 -flto)
else()
	add_compile_options(-gsource-map)
	add_link_options(
		-gsource-map
		-sDEMANGLE_SUPPORT=1
		-sASSERTIONS=2
		-sNO_EXIT_RUNTIME=0 
		-sSAFE_HEAP=0)
endif()
