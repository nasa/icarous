cmake_minimum_required(VERSION 3.9)
project(Astar)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS -m32)

set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../lib)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../ACCoRD/inc)

link_directories(${LIBRARY_OUTPUT_PATH})

add_executable(Astar main.cpp Node.cpp Node.h Astar.cpp Astar.h)
target_link_libraries(Astar ACCoRD)