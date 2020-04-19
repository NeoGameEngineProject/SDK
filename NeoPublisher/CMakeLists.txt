file(GLOB_RECURSE HEADERS include/*.h)
file(GLOB_RECURSE SOURCES src/*.c*)

add_library(NeoPublisher STATIC
	${SOURCES}
	${HEADERS}
)

target_link_libraries(NeoPublisher PRIVATE NeoEngine)
target_include_directories(NeoPublisher PUBLIC include)

add_executable(publisher main.cpp)
target_link_libraries(publisher PRIVATE NeoPublisher)

if(NOT NO_TESTS)
	file(GLOB TEST_SOURCES tests/*.c*)
	add_executable(PublisherTest ${TEST_SOURCES})
	target_link_libraries(PublisherTest ${GTEST_LIBRARIES} NeoPublisher)
endif()
