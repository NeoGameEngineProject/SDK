## Find source and headers
file(GLOB HEADERS include/*.h)
file(GLOB SOURCES src/*.c*)

set(CMAKE_SHARED_LIBRARY_PREFIX "")
add_subdirectory(swig)

add_library(NeoEngine ${SOURCES} ${HEADERS})

target_compile_definitions(NeoEngine PRIVATE -DNEO_CORE_DLL)
target_include_directories(NeoEngine PUBLIC include)
target_link_libraries(NeoEngine PUBLIC NeoCore ${CMAKE_DL_LIBS})

if(NOT NO_TESTS)
    add_executable(NeoEngineTest tests/main.cpp)
    target_link_libraries(NeoEngineTest ${GTEST_LIBRARIES} NeoEngine)
endif()

