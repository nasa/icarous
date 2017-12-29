cmake_minimum_required(VERSION 2.6.4)
project(GEOFENCE_APP C)

set(CMAKE_CXX_STANDARD 11)


set(MODULES_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../../C++/Modules")
set(MODULES_LIB "${CMAKE_CURRENT_SOURCE_DIR}/../../../C++/lib")
set(PLEXIL_SOURCE "${MODULES_ROOT}/Plexil/src")

include_directories(../inc)
include_directories(${MODULES_ROOT}/GeofenceMonitor)
include_directories(${MODULES_ROOT}/GeofenceMonitor/CWrapper)
include_directories(${PLEXIL_SOURCE}/interfaces/CfsAdapter)

aux_source_directory(fsw/src APP_SRC_FILES)

link_directories(${MODULES_LIB})

add_library(geofence MODULE ${APP_SRC_FILES} ${MODULES_ROOT}/GeofenceMonitor/CWrapper/GeofenceMonitor_proxy.c)

target_link_libraries(geofence GeofenceMonitor)

set_target_properties(geofence PROPERTIES PREFIX "")

set_target_properties(geofence PROPERTIES INSTALL_RPATH "${MODULES_LIB}")

add_dependencies(geofence GeofenceMonitor-all)

foreach(TGT ${APP_INSTALL_LIST})
    install(TARGETS geofence DESTINATION ${TGT}/${INSTALL_SUBDIR})
endforeach()
