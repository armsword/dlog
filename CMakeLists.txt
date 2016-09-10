PROJECT(DLOG)

CMAKE_MINIMUM_REQUIRED (VERSION 2.6)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
endif()

set(CXX_FLAGS
 -g
 -Wall
 -Werror
 )

string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")


set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS_DEBUG "-O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

include_directories(${PROJECT_SOURCE_DIR})

add_subdirectory(dlog/common)
add_subdirectory(dlog/config)
add_subdirectory(dlog/logger)

add_subdirectory(example)
add_subdirectory(perf_test)
