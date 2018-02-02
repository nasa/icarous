cmake_minimum_required(VERSION 2.6.4)
project(GEOFENCE_APP C)

set(CMAKE_CXX_STANDARD 11)


set(MODULES_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../../Modules")
set(MODULES_LIB "${MODULES_ROOT}/lib")
set(PLEXIL_SOURCE "${MODULES_ROOT}/Plexil/src")
set(PLEXIL_LIB "${PLEXIL_SOURCE}/../lib")


include_directories(../inc)
include_directories(${MODULES_ROOT}/GeofenceMonitor)
include_directories(${MODULES_ROOT}/GeofenceMonitor/CWrapper)
include_directories(${PLEXIL_SOURCE}/interfaces/CfsAdapter)
include_directories(${PLEXIL_SOURCE}/wrapper/)

aux_source_directory(fsw/src APP_SRC_FILES)

link_directories(${MODULES_LIB})
link_directories(${PLEXIL_LIB})

add_library(geofence MODULE ${APP_SRC_FILES} ${MODULES_ROOT}/GeofenceMonitor/CWrapper/GeofenceMonitor_proxy.c)

target_link_libraries(geofence GeofenceMonitor PlexilWrapper)

set_target_properties(geofence PROPERTIES PREFIX "")

set_target_properties(geofence PROPERTIES INSTALL_RPATH "${MODULES_LIB}:${PLEXIL_LIB}")

foreach(TGT ${APP_INSTALL_LIST})
    install(TARGETS geofence DESTINATION ${TGT}/${INSTALL_SUBDIR})
endforeach()
