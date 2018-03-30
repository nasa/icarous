cmake_minimum_required(VERSION 3.9)
project(BSplines)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "-DHAVE_STD -DHAVE_NAMESPACES")

add_executable(BSplines Bsplines.cpp main.cpp)

target_link_libraries(BSplines opt newmat blas gfortran m gcc_s quadmath)