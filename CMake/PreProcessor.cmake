
macro(preprocess_file input output)

	get_filename_component(_CWD ${input} DIRECTORY)
	get_filename_component(_NAME ${input} NAME)

	get_filename_component(_OUTDIR ${output} DIRECTORY)

	file(MAKE_DIRECTORY ${_OUTDIR})

	if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"
			OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")

		if(NOT WIN32)
			add_custom_command(OUTPUT ${output}
				                COMMAND cpp -nostdinc -CC -P -E ${input} | sed "s/\\$/\#/g" > ${output}
								WORKING_DIRECTORY ${_CWD}
								DEPENDS ${input}
								COMMENT "Preprocessing shader ${_NAME}" VERBATIM)
		else()
			add_custom_command(OUTPUT ${output}
				                COMMAND cpp -nostdinc -CC -P -E ${input} > ${output}.tmp
								COMMAND PowerShell -Command "get-content ${output}.tmp | %{$_ -replace \"\\$\",\"\#\"} | out-file \"${output}\" -encoding ascii"
								WORKING_DIRECTORY ${_CWD}
								DEPENDS ${input}
								COMMENT "Preprocessing shader ${_NAME}" VERBATIM)
		endif()

		set_source_files_properties(${output} PROPERTIES GENERATED TRUE)

	elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
		add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${output}
				COMMAND ${CMAKE_CXX_COMPILER} /C /X /EP ${input} > ${CMAKE_CURRENT_BINARY_DIR}/${output}
				WORKING_DIRECTORY ${_CWD}
				DEPENDS ${input}
				COMMENT "Preprocessing shader ${_NAME}")
	else()
		message(FATAL_ERROR "Unknown compiler, don't know to to invoke preprocessor!")
	endif()

endmacro()

function(preprocess_files)
	cmake_parse_arguments(ARG "" "" "SOURCES;INCLUDE_DIRECTORIES" ${ARGN})
	set(_PROCESSED_SHADERS "")

	foreach(FILE ${ARG_SOURCES})
		message(${FILE})
		get_filename_component(NAME ${FILE} NAME_WE)
		set(OUTFILE ${CMAKE_BINARY_DIR}/bin/assets/materials/builtin/${NAME}.glsl)
		preprocess_file(${FILE} ${OUTFILE})
		set(_PROCESSED_SHADERS ${_PROCESSED_SHADERS} ${OUTFILE})
	endforeach()

	set(PROCESSED_SHADERS ${_PROCESSED_SHADERS} PARENT_SCOPE)
endfunction()
