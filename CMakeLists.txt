cmake_minimum_required(VERSION 3.6)
project(timeit)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(external/googletest)

add_executable(timeit_demo timeit_demo.cpp timeit.h)

add_test(timeit_test timeit_test.cpp)
add_executable(timeit_test timeit_test.cpp timeit.h)
target_link_libraries(timeit_test gtest gmock gmock_main)