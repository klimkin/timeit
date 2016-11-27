cmake_minimum_required(VERSION 3.4)
project(timeit)

enable_testing()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
endif ()

add_subdirectory(external/googletest)
add_subdirectory(examples)

add_executable(timeit_demo timeit_demo.cpp timeit.h)

add_executable(timeit_test timeit_test.cpp timeit.h)
add_test(timeit_test timeit_test)
target_link_libraries(timeit_test gtest gmock gmock_main)
