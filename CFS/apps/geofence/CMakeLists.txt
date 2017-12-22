cmake_minimum_required(VERSION 2.6.4)
project(GEOFENCE_APP C)

set(CMAKE_CXX_STANDARD 11)

set(PLEXIL_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/../../../C++/plexil-4/src")
set(MODULES_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../../C++/Modules")
set(MODULES_LIB "${CMAKE_CURRENT_SOURCE_DIR}/../../../C++/lib")

include_directories(../inc)
include_directories(${MODULES_ROOT}/GeofenceMonitor)
include_directories(${MODULES_ROOT}/GeofenceMonitor/CWrapper)
include_directories(${PLEXIL_SOURCE}/interfaces/CfsAdapter)

message(STATUS "fd source: ${MODULES_ROOT}/GeofenceMonitor")

aux_source_directory(fsw/src APP_SRC_FILES)

link_directories(${MODULES_LIB})

add_library(geofence MODULE ${APP_SRC_FILES})

target_link_libraries(geofence FlightDataManager GeofenceMonitor)

set_target_properties(geofence PROPERTIES PREFIX "")

set_target_properties(geofence PROPERTIES INSTALL_RPATH "${MODULES_LIB}")

add_dependencies(geofence FlightDataManager-all GeofenceMonitor-all)

foreach(TGT ${APP_INSTALL_LIST})
    install(TARGETS geofence DESTINATION ${TGT}/${INSTALL_SUBDIR})
endforeach()
