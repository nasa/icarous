cmake_minimum_required(VERSION 2.6)
project(DubinsPlanner)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES DubinsPlanner.cpp SearchAlgorithms.cpp)

set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../lib)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../ACCoRD/inc)

link_directories(${LIBRARY_OUTPUT_PATH})

add_library(DubinsPlanner SHARED ${SOURCE_FILES})

target_link_libraries(DubinsPlanner ACCoRD)