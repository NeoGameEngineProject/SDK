file(GLOB SOURCES src/*.c*)
file(GLOB INCLUDES include/*.h)

add_subdirectory(libpng EXCLUDE_FROM_ALL)
add_library(textureload ${SOURCES} ${INCLUDES})

target_link_libraries(textureload PUBLIC NeoCore NeoEngine png_static)
target_include_directories(textureload PUBLIC include)

if(NOT NO_TESTS)
	file(GLOB TEST_SOURCES tests/*.c*)
	add_executable(TextureLoadTest ${TEST_SOURCES})
	target_link_libraries(TextureLoadTest ${GTEST_LIBRARIES} platform NeoEngine)
endif()
