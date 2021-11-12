cmake_minimum_required(VERSION 2.6)
project(TargetTracker)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../lib)
set(CMAKE_SHARED_LIBRARY_SUFFIX ".so")
set(SOURCE_FILES TargetTracker.cpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../ACCoRD/inc)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/eigen)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

link_directories(${LIBRARY_OUTPUT_PATH})

add_library(TargetTracker SHARED ${SOURCE_FILES})

target_link_libraries(TargetTracker ACCoRD)

add_executable(trackerTest test/main.cpp)
target_link_libraries(trackerTest TargetTracker)