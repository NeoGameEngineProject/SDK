## Find source and headers
file(GLOB HEADERS include/*.h)
file(GLOB SOURCES src/*.c*)

set(CMAKE_SHARED_LIBRARY_PREFIX "")

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

add_library(NeoCore ${SOURCES} ${HEADERS})

if(NOT NO_TESTS)
  add_executable(NeoCoreTest tests/main.cpp tests/ThreadPoolTest.cpp)

  ## Travis does not like TARGET_include_directories
  include_directories(${GTEST_INCLUDE_DIR})
  target_link_libraries(NeoCoreTest ${GTEST_LIBRARIES} NeoCore)
endif()

if(NOT BUILD_SHARED_LIBS)
	target_compile_definitions(NeoCore PUBLIC -DNEO_CORE_STATIC)
endif()

target_compile_definitions(NeoCore PRIVATE -DNEO_CORE_DLL)
<<<<<<< HEAD
target_include_directories(NeoCore PUBLIC include PRIVATE src)
=======
target_include_directories(NeoCore PUBLIC include)
target_link_libraries(NeoCore PUBLIC ${CMAKE_THREAD_LIBS_INIT})
>>>>>>> + [NEOCORE] Implemented thread pool
