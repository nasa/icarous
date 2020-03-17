cmake_minimum_required(VERSION 2.6)
project(ACCoRD)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_SHARED_LIBRARY_SUFFIX ".so")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../lib)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src SRC_FILES)

set(SOURCE_FILES ${SRC_FILES})

include_directories(inc)

add_library(ACCoRD SHARED ${SOURCE_FILES})
