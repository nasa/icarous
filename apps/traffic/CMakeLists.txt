cmake_minimum_required(VERSION 2.6.4)
project(TRAFFIC_APP C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")

set(CMAKE_CXX_STANDARD 11)

set(MODULES_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../Modules")
set(MODULES_LIB "${MODULES_ROOT}/lib")

include_directories(../Icarouslib/fsw/platform_inc)
include_directories(../scheduler/fsw/platform_inc)
include_directories(../trajectory/fsw/platform_inc)
include_directories(${MODULES_ROOT}/Core/Utils)
include_directories(${MODULES_ROOT}/Core/Interfaces)
include_directories(${MODULES_ROOT}/Core/TrafficMonitor)
include_directories(${MODULES_ROOT}/Core/TrafficMonitor/CWrapper)
include_directories(fsw/src)
include_directories(fsw/tables)
include_directories(fsw/platform_inc)

aux_source_directory(fsw/src APP_SRC_FILES)

link_directories(${MODULES_LIB})

add_library(traffic MODULE ${APP_SRC_FILES})

target_link_libraries(traffic TrafficMonitor Utils)

set_target_properties(traffic PROPERTIES PREFIX "")

set_target_properties(traffic PROPERTIES INSTALL_RPATH "${MODULES_LIB}")

#add_cfe_tables(traffic "fsw/tables/traffic_tbl.c")

foreach(TGT ${APP_INSTALL_LIST})
    install(TARGETS traffic DESTINATION ${TGT}/${INSTALL_SUBDIR})
endforeach()
