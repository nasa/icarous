cmake_minimum_required(VERSION 2.6)
project(GeofenceMonitor)

#set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")
set(CMAKE_SHARED_LIBRARY_SUFFIX ".so")
set(SOURCE_FILES GeofenceMonitor.cpp CWrapper/GeofenceMonitor_wrap.cxx CWrapper/GeofenceMonitor_proxy.c)

set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../lib)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/CWrapper)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../ACCoRD/inc)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../Utils)

link_directories(${LIBRARY_OUTPUT_PATH})

add_library(GeofenceMonitor SHARED ${SOURCE_FILES})

target_link_libraries(GeofenceMonitor Utils ACCoRD)

add_executable(GeofenceTest Test/main.cpp)

target_link_libraries(GeofenceTest Utils ACCoRD GeofenceMonitor)

add_executable(GeofenceTestC Test/main.c CWrapper/GeofenceMonitor_proxy.c)
target_link_libraries(GeofenceTestC GeofenceMonitor)
