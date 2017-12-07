cmake_minimum_required(VERSION 3.7)
project(FlightDataManager)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "-g -fPIC -m32")

set(SOURCE_FILES FlightData.cpp FlightDataWrapper.cpp GenericObject.cpp Geofence.cpp)

set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../lib)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include/ACCoRD
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/ICAROUS)

link_directories(${LIBRARY_OUTPUT_PATH})

add_library(FlightDataManager SHARED ${SOURCE_FILES})

target_link_libraries(FlightDataManager ACCoRD)