cmake_minimum_required(VERSION 2.6)
project(Cognition)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../lib)
set(CMAKE_SHARED_LIBRARY_SUFFIX ".so")
set(SOURCE_FILES Cognition.cpp Wrappers.cpp CognitionUtil.cpp CoreLogic.cpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../ACCoRD/inc)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../Interfaces)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../Utils)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../EventManager)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

link_directories(${LIBRARY_OUTPUT_PATH})

add_library(Cognition SHARED ${SOURCE_FILES})

target_link_libraries(Cognition Utils ACCoRD)
