set(JAVA_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/java)
set(MONO_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/mono)

file(REMOVE ${JAVA_OUTPUT})
file(MAKE_DIRECTORY ${JAVA_OUTPUT})

file(REMOVE ${MONO_OUTPUT})
file(MAKE_DIRECTORY ${MONO_OUTPUT})

find_package(SWIG)
include(UseSWIG)

include_directories(../include ../../NeoCore/include)

if (${SWIG_FOUND})
    message("-- Generating script bindings for NeoEngine")

    set(CMAKE_SHARED_LIBRARY_PREFIX "")
    if (${CSHARP_FOUND})
        if (WIN32)
            set(DLLIMPORT ./NeoSharpNativeEngine.dll)
        else ()
            set(DLLIMPORT ./NeoSharpNativeEngine.so)
        endif ()

        set_source_files_properties(NeoSharpEngine.i PROPERTIES CPLUSPLUS ON)
        set(CMAKE_SWIG_OUTDIR ${MONO_OUTPUT})
        set(SWIG_ARGS -I${NEO_CORE_INCLUDE_DIR} -I${NEO_ENGINE_INCLUDE_DIR} -dllimport ${DLLIMPORT} -Doverride)
        set_property(SOURCE NeoSharpEngine.i PROPERTY SWIG_MODULE_NAME Neo)

        swig_add_library(NeoSharpEngine LANGUAGE csharp SOURCES NeoSharpEngine.i)

        swig_link_libraries(NeoSharpEngine NeoEngine ${MONO_LIBRARIES})
        target_include_directories(NeoSharpEngine PUBLIC ${MONO_INCLUDE_DIR})
        add_custom_target(NeoSharpEngineCLI ALL ${CSHARP_COMPILER} -t:library -out:${NEO_BINARY_OUTPUT}/NeoSharpEngine.dll ${MONO_OUTPUT}/*.cs)

        add_dependencies(NeoSharpEngineCLI NeoSharpEngine)
    endif (${CSHARP_FOUND})

    IF (${Java_FOUND} AND ${JNI_FOUND})

        set_source_files_properties(NeoEngineJava.i PROPERTIES CPLUSPLUS ON)
        set(CMAKE_SWIG_OUTDIR ${JAVA_OUTPUT})
        set(CMAKE_SWIG_FLAGS -DSWIG_JAVA -Doverride)
        set_property(SOURCE NeoEngineJava.i PROPERTY SWIG_MODULE_NAME Neo)

        swig_add_library(NeoEngineJava LANGUAGE java SOURCES NeoEngineJava.i)

        swig_link_libraries(NeoEngineJava NeoEngine ${JNI_LIBRARIES})
        target_include_directories(NeoEngineJava PUBLIC ${JNI_INCLUDE_DIRS})

        ## Execute during configuration so it can generate the Java files needed
        execute_process(
                COMMAND ${SWIG_EXECUTABLE} -Doverride -java -c++ ${CMAKE_SWIG_FLAGS} -outdir ${CMAKE_SWIG_OUTDIR}
                -I${NEO_CORE_INCLUDE_DIR} -I${NEO_ENGINE_INCLUDE_DIR}
                -o ${CMAKE_CURRENT_BINARY_DIR}/NeoEngineJava_wrap.cxx -package NeoEngine -DSWIG_JAVA

                ${CMAKE_CURRENT_SOURCE_DIR}/NeoEngineJava.i
                OUTPUT_VARIABLE SWIG_swiglib_output
                ERROR_VARIABLE SWIG_swiglib_error
                RESULT_VARIABLE SWIG_swiglib_result
        )

        file(GLOB JAVA_SOURCES ${JAVA_OUTPUT}/*.java)
        add_jar(NeoEngineJar ${JAVA_SOURCES})
        add_dependencies(NeoEngineJar NeoEngineJava)

        install(TARGETS libNeoEngineJava DESTINATION ${NEO_INSTALL_DIR})
        install(FILES ${CMAKE_CURRENT_BINARY_DIR}/NeoEngineJava.jar DESTINATION ${NEO_INSTALL_DIR})
    endif (${Java_FOUND} AND ${JNI_FOUND})

    if (NOT ${SWIG_swiglib_result} EQUAL 0)
        message(SEND_ERROR "-- Generating script bindings for Neo failed with output:\n${SWIG_swiglib_error}")
    endif (NOT ${SWIG_swiglib_result} EQUAL 0)

    set_source_files_properties(NeoEngineLua.i PROPERTIES CPLUSPLUS ON)
    set(CMAKE_SWIG_FLAGS -Doverride)

    swig_add_library(NeoEngineLua LANGUAGE lua SOURCES NeoEngineLua.i)

    swig_link_libraries(NeoEngineLua NeoEngine ${LUA_LIBRARIES})
    target_include_directories(NeoEngineLua PUBLIC ${LUA_INCLUDE_DIR})

else ()
    message("-- Will NOT generate script bindings for NeoEngine")
endif ()
