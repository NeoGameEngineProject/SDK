## Find source and headers
file(GLOB HEADERS include/*.h)
file(GLOB SOURCES src/*.c*)

set(CMAKE_SHARED_LIBRARY_PREFIX "")

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

if(NEO_BUILD_SHARED_LIBS)
	add_library(NeoCore SHARED ${SOURCES} ${HEADERS})
else()
	add_library(NeoCore STATIC ${SOURCES} ${HEADERS})
endif()

if(NOT NO_TESTS)
  add_executable(NeoCoreTest tests/main.cpp tests/ThreadPoolTest.cpp tests/LogTest.cpp tests/FixedStringTest.cpp)

  ## Travis does not like TARGET_include_directories
  include_directories(${GTEST_INCLUDE_DIR})
  target_link_libraries(NeoCoreTest ${GTEST_LIBRARIES} NeoCore)
endif()

if(NOT BUILD_SHARED_LIBS)
	target_compile_definitions(NeoCore PUBLIC -DNEO_CORE_STATIC)
endif()

if(EMSCRIPTEN OR NEO_SINGLE_THREAD)
	target_compile_definitions(NeoCore PUBLIC -DNEO_SINGLE_THREAD)
endif()

target_compile_definitions(NeoCore PRIVATE -DNEO_CORE_DLL)
target_include_directories(NeoCore PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>  $<INSTALL_INTERFACE:include>)

if(WIN32)
	target_include_directories(NeoCore PRIVATE dirent)
endif()

target_link_libraries(NeoCore PUBLIC ${CMAKE_THREAD_LIBS_INIT})

install(TARGETS NeoCore
		EXPORT NeoCore
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib)
	
install(EXPORT NeoCore DESTINATION ${NEO_CMAKE_EXPORT})
install(DIRECTORY include/ DESTINATION include/)
