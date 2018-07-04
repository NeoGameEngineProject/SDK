
function(add_platform_module)
	cmake_parse_arguments(ARG "" "" "SOURCES;INCLUDES;LIBRARIES" ${ARGN})

	get_property(tmp GLOBAL PROPERTY NEO_PLATFORM_INCS_PROP)
	set(tmp "${tmp};${ARG_INCLUDES}")
	set_property(GLOBAL PROPERTY NEO_PLATFORM_INCS_PROP "${tmp}")
	
	get_property(tmp GLOBAL PROPERTY NEO_PLATFORM_SRCS_PROP)
	set(tmp "${tmp};${ARG_SOURCES}")
	set_property(GLOBAL PROPERTY NEO_PLATFORM_SRCS_PROP "${tmp}")
	
	get_property(tmp GLOBAL PROPERTY NEO_PLATFORM_LIBS_PROP)
	set(tmp "${tmp};${ARG_LIBRARIES}")
	set_property(GLOBAL PROPERTY NEO_PLATFORM_LIBS_PROP "${tmp}")
endfunction()
