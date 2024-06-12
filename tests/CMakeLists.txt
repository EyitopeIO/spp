cmake_minimum_required(VERSION 3.10)

# Set the project name
project(sppTests)

enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
include_directories(../src)
file(GLOB TEST_SOURCES "*.cpp")
file(GLOB_RECURSE SOURCES "../src/*.cpp" "../src/*.c")
list(FILTER SOURCES EXCLUDE REGEX ".*main.cpp")
add_executable(${PROJECT_NAME} ${TEST_SOURCES} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${GTEST_LIBRARIES} gtest_main pthread)
add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
add_custom_target(tests COMMAND ${CMAKE_CTEST_COMMAND} -V)