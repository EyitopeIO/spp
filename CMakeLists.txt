cmake_minimum_required(VERSION 3.10)

project(spp VERSION 1.0)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.c")
add_executable(spp ${SOURCES})
if(DEFINED ENV{DEBUG})
  target_compile_definitions(spp PUBLIC -DDEBUG)
  target_compile_options(spp PUBLIC -g -O0)
endif()
target_compile_options(spp PUBLIC -Wall)
add_subdirectory(tests)

