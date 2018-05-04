cmake_minimum_required(VERSION 3.5)
project(Astar)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS -m32)

set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../lib)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../ACCoRD/inc)

link_directories(${LIBRARY_OUTPUT_PATH})

set(SOURCE_FILES Astar.cpp Node.cpp)

add_library(ASTAR SHARED ${SOURCE_FILES})

target_link_libraries(ASTAR ACCoRD)

add_executable(Astar main.cpp)

target_link_libraries(Astar ASTAR)