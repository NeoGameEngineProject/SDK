find_program(CAT_EXEC cat)
if(${CAT_EXEC} STREQUAL "cat-NOTFOUND")
	set(USE_CAT OFF)
else()
	set(USE_CAT ON)
endif()

macro(add_game name sources assetdir)
	add_executable(${name} ${sources})
	target_link_libraries(${name} PUBLIC NeoEngine NeoHTML NeoStates)
	
	if("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
		target_compile_definitions(${name} PRIVATE -DASSET_MODE=1)
		if(${USE_CAT})
			add_custom_command(TARGET ${name} POST_BUILD
				## Zip assets
				COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets/glsl ${assetdir}/glsl ## TODO Find real shader output!
				COMMAND ${CMAKE_COMMAND} -E tar "cf" "${CMAKE_CURRENT_BINARY_DIR}/data.neo" --format=zip ${assetdir}
				COMMAND ${CMAKE_COMMAND} -E remove_directory  ${assetdir}/glsl

				## Cat files
				COMMAND ${CAT_EXEC} $<TARGET_FILE:${name}> "${CMAKE_CURRENT_BINARY_DIR}/data.neo" > ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${name}-full
				
				## Somebody with cat also has chmod! (not really... :()
				COMMAND chmod +x ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${name}-full
				WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
				COMMENT "Building fat binary"
			)
		else()
			add_custom_command(TARGET ${name} POST_BUILD
				## Zip assets
				COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets/glsl ${assetdir}/glsl ## TODO Find real shader output!
				COMMAND ${CMAKE_COMMAND} -E tar "cfv" "${CMAKE_CURRENT_BINARY_DIR}/data.neo" --format=zip ${assetdir}
				COMMAND ${CMAKE_COMMAND} -E remove_directory  ${assetdir}/glsl
				
				## Cat files
				COMMAND copy /b $<TARGET_FILE:${name}>+"${CMAKE_CURRENT_BINARY_DIR}/data.neo" ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${name}-full.exe
				WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
				COMMENT "Building fat binary"
			)
		endif()
	else() ## For debug builds we just copy the built shaders to the assets directory so the game can be run there
		target_compile_definitions(${name} PRIVATE -DASSET_MODE=0)
		add_custom_command(TARGET ${name} POST_BUILD
				COMMAND ${CMAKE_COMMAND} -E remove_directory ${assetdir}/glsl ## TODO Find real shader output!
				COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets/glsl ${assetdir}/glsl ## TODO Find real shader output!
				WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
				COMMENT "Copying shaders"
			)
	endif()
endmacro()
