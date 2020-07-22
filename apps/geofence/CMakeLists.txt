cmake_minimum_required(VERSION 2.6.4)
project(GEOFENCE_APP C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")

set(CMAKE_CXX_STANDARD 11)


set(MODULES_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../Modules")
set(MODULES_LIB "${MODULES_ROOT}/lib")

include_directories(${MODULES_ROOT}/Core/Interfaces)
include_directories(${MODULES_ROOT}/Core/GeofenceMonitor)
include_directories(../Icarouslib/fsw/platform_inc)
include_directories(../trajectory/fsw/platform_inc)
include_directories(../scheduler/fsw/platform_inc)
include_directories(fsw/src)
include_directories(fsw/tables)
include_directories(fsw/platform_inc)

aux_source_directory(fsw/src APP_SRC_FILES)

link_directories(${MODULES_LIB})

add_library(geofence MODULE ${APP_SRC_FILES})

target_link_libraries(geofence GeofenceMonitor)

set_target_properties(geofence PROPERTIES PREFIX "")

set_target_properties(geofence PROPERTIES INSTALL_RPATH "${MODULES_LIB}")

#add_cfe_tables(geofence "fsw/tables/geofence_tbl.c")

foreach(TGT ${APP_INSTALL_LIST})
    install(TARGETS geofence DESTINATION ${TGT}/${INSTALL_SUBDIR})
endforeach()
