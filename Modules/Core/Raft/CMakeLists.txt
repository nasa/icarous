cmake_minimum_required(VERSION 2.6)
project(Raft)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")
set(CMAKE_SHARED_LIBRARY_SUFFIX ".so")

set(SOURCE_FILES RaftCore.cpp) 

set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../lib)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../ACCoRD/inc
                    ${CMAKE_CURRENT_SOURCE_DIR}/../Utils
                    ${CMAKE_CURRENT_SOURCE_DIR}/../Interfaces)

link_directories(${LIBRARY_OUTPUT_PATH})

add_library(Raft SHARED ${SOURCE_FILES})

target_link_libraries(Raft ACCoRD Utils)

