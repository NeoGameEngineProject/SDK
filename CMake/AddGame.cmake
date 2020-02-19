find_program(CAT_EXEC cat)
if(${CAT_EXEC} STREQUAL "cat-NOTFOUND")
	set(USE_CAT OFF)
else()
	set(USE_CAT ON)
endif()

macro(add_game name sources assetdir)
	add_executable(${name} WIN32 ${sources})
	target_link_libraries(${name} PUBLIC NeoEngine NeoCore NeoHTML NeoVR NeoStates NeoScript)
	
	if(ENABLE_SANITIZERS)
		target_compile_options(${name} PUBLIC -fsanitize=${SANITIZER} -fsanitize=undefined -fno-omit-frame-pointer)
	endif()
	
	if(NOT EMSCRIPTEN)
		add_custom_target(build-package-${name} 
			COMMAND ${CMAKE_COMMAND} -E tar "cf" "${CMAKE_CURRENT_BINARY_DIR}/data.neo" --format=zip ${assetdir}
			WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
			COMMENT "Building package")
		
		if(NOT ENABLE_OPENGL_RENDERER)
			add_custom_target(copy-shaders
				COMMAND ${CMAKE_COMMAND} -E remove_directory ${assetdir}/glsl ## TODO Find real shader output!
				COMMAND ${CMAKE_COMMAND} -E remove_directory ${assetdir}/asm.js ## TODO Find real shader output!

				COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets/glsl ${assetdir}/glsl ## TODO Find real shader output!
				COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets/asm.js ${assetdir}/asm.js ## TODO Find real shader output!

				WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
				COMMENT "Copying shaders"
				DEPENDS build-shaders
			)
			add_dependencies(${name} copy-shaders)
			add_dependencies(build-package-${name} copy-shaders)
		else()
			add_custom_target(copy-shaders
				COMMAND ${CMAKE_COMMAND} -E remove_directory ${assetdir}/materials ## TODO Find real shader output!
				COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets/materials ${assetdir}/materials
				
				WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
				COMMENT "Copying shaders"
				DEPENDS build-shaders
			)
			add_dependencies(${name} copy-shaders)
			add_dependencies(build-package-${name} copy-shaders)
		endif()
	else()
		add_custom_target(build-package-${name} 
			COMMAND ${CMAKE_COMMAND} -E tar "cf" "${CMAKE_BINARY_DIR}/bin/data.neo" --format=zip ${assetdir}
			COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/scripts/logo.png ${CMAKE_BINARY_DIR}/bin/ ## TODO Find real shader output!
			WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
			COMMENT "Building package")
		add_dependencies(${name} build-package-${name})
	endif()
	
	if("${CMAKE_BUILD_TYPE}" STREQUAL "Release" AND NOT EMSCRIPTEN)
		target_compile_definitions(${name} PUBLIC -DASSET_MODE=1)
		target_compile_definitions(NeoEngine PUBLIC -DASSET_MODE=1)

		if(${USE_CAT})
			if(NOT ENABLE_OPENGL_RENDERER)
				add_custom_command(TARGET ${name} POST_BUILD
					## Zip assets
					COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets/glsl ${assetdir}/glsl ## TODO Find real shader output!
					COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets/asm.js ${assetdir}/asm.js
					
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
					COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets/materials ${assetdir}/materials ## TODO Find real shader output!
					
					COMMAND ${CMAKE_COMMAND} -E tar "cf" "${CMAKE_CURRENT_BINARY_DIR}/data.neo" --format=zip ${assetdir}
					COMMAND ${CMAKE_COMMAND} -E remove_directory  ${assetdir}/materials

					## Cat files
					COMMAND ${CAT_EXEC} $<TARGET_FILE:${name}> "${CMAKE_CURRENT_BINARY_DIR}/data.neo" > ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${name}-full
					
					## Somebody with cat also has chmod! (not really... :()
					COMMAND chmod +x ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${name}-full
					WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
					COMMENT "Building fat binary"
				)
			endif()
		else()
			if(NOT ENABLE_OPENGL_RENDERER)
				add_custom_command(TARGET ${name} POST_BUILD
					## Zip assets
					COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets/glsl ${assetdir}/glsl ## TODO Find real shader output!
					COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets/asm.js ${assetdir}/asm.js
					COMMAND ${CMAKE_COMMAND} -E tar "cf" "${CMAKE_CURRENT_BINARY_DIR}/data.neo" --format=zip ${assetdir}
					COMMAND ${CMAKE_COMMAND} -E remove_directory  ${assetdir}/glsl
					
					## Cat files
					COMMAND copy /b $<TARGET_FILE:${name}>+"${CMAKE_CURRENT_BINARY_DIR}/data.neo" ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${name}-full.exe
					WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
					COMMENT "Building fat binary"
				)
			else()
				add_custom_command(TARGET ${name} POST_BUILD
					## Zip assets
					COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets/materials ${assetdir}/materials ## TODO Find real shader output!
					COMMAND ${CMAKE_COMMAND} -E tar "cf" "${CMAKE_CURRENT_BINARY_DIR}/data.neo" --format=zip ${assetdir}
					COMMAND ${CMAKE_COMMAND} -E remove_directory  ${assetdir}/materials
					
					## Cat files
					COMMAND copy /b $<TARGET_FILE:${name}>+"${CMAKE_CURRENT_BINARY_DIR}/data.neo" ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${name}-full.exe
					WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
					COMMENT "Building fat binary"
				)
			endif()
		endif()
	else() ## For debug builds we just copy the built shaders to the assets directory so the game can be run there
		target_compile_definitions(${name} PRIVATE -DASSET_MODE=0)
	endif()
endmacro()
