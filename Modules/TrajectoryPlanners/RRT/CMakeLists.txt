cmake_minimum_required(VERSION 3.5)
project(RRT)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES RRTplanner.cpp)

set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../lib)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../ACCoRD/inc)

link_directories(${LIBRARY_OUTPUT_PATH})

add_library(RRT SHARED ${SOURCE_FILES})

target_link_libraries(RRT ACCoRD)

add_executable(RRTtest main.cpp)

target_link_libraries(RRTtest RRT)