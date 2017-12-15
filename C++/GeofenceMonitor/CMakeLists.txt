cmake_minimum_required(VERSION 3.7)
project(GeofenceMonitor)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "-g -fPIC -m32")

set(SOURCE_FILES GeofenceMonitor.cpp)

set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../lib)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include/ACCoRD)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../FlightDataManager)

link_directories(${LIBRARY_OUTPUT_PATH})

add_library(GeofenceMonitor SHARED ${SOURCE_FILES})

target_link_libraries(GeofenceMonitor FlightDataManager ACCoRD)