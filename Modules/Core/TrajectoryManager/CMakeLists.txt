cmake_minimum_required(VERSION 2.6)
project(TrajectoryManager)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")
set(CMAKE_SHARED_LIBRARY_SUFFIX ".so")
add_subdirectory(DubinsPlanner)

set(SOURCE_FILES TrajManager.cpp TrajManagerWrapper.cpp)

set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../lib)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../ACCoRD/inc
                    ${CMAKE_CURRENT_SOURCE_DIR}/../Utils
                    ${CMAKE_CURRENT_SOURCE_DIR}/../Interfaces
                    ${CMAKE_CURRENT_SOURCE_DIR}/DubinsPlanner)

link_directories(${LIBRARY_OUTPUT_PATH})

add_library(TrajectoryManager SHARED ${SOURCE_FILES})

target_link_libraries(TrajectoryManager DubinsPlanner ACCoRD Utils)

