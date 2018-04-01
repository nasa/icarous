cmake_minimum_required(VERSION 3.9)
project(BSplines)

set(CMAKE_CXX_STANDARD 11)

add_executable(BSplines Bsplines.cpp main.cpp)

target_link_libraries(BSplines m nlopt)