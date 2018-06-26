cmake_minimum_required(VERSION 2.6)
project(BSplines)

set(CMAKE_CXX_STANDARD 11)

set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../lib)

link_directories(${LIBRARY_OUTPUT_PATH})
find_library(NLOPTLIB nlopt HINTS $ENV{NLOPT_PATH})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../ACCoRD/inc)

add_library(BSPLINES SHARED Bsplines.cpp kdtree.cpp)

target_link_libraries(BSPLINES m ${NLOPTLIB} ACCoRD)

add_executable(BSplines main.cpp)

target_link_libraries(BSplines BSPLINES)
