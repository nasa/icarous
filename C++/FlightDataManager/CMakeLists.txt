cmake_minimum_required(VERSION 3.7)
project(FlightDataManager)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES FlightData.cpp FlightData.h)

set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../lib)

set(CMAKE_CXX_FLAGS "-g -fPIC -m32")

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/ICAROUS
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/ACCoRD)

link_directories(${LIBRARY_OUTPUT_PATH})

add_library(FlightDataManager SHARED ${SOURCE_FILES})

target_link_libraries(FlightDataManager ACCoRD)