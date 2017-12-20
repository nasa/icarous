cmake_minimum_required(VERSION 3.7)
project(TrafficMonitor)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "-g -fPIC -m32")

set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../lib)

set(SOURCE_FILES TrafficMonitor.cpp CWrapper/TrafficMonitor_wrap.cxx)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../ACCoRD/inc)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../FlightDataManager)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

link_directories(${LIBRARY_OUTPUT_PATH})

add_library(TrafficMonitor SHARED ${SOURCE_FILES})

target_link_libraries(TrafficMonitor ACCoRD FlightDataManager)

add_executable(trafficTest Test/main.cpp)
target_link_libraries(trafficTest TrafficMonitor)