cmake_minimum_required(VERSION 3.5)
project(FlightDataManager)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES FlightData.cpp FlightDataWrapper.cpp GenericObject.cpp fence.cpp)

set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../lib)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../ACCoRD/inc)

link_directories(${LIBRARY_OUTPUT_PATH})

add_library(FlightDataManager SHARED ${SOURCE_FILES})

target_link_libraries(FlightDataManager ACCoRD)