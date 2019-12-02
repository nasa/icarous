cmake_minimum_required(VERSION 2.6.4)
project(CFE_ARDUPILOT_INTERFACE C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")

set(MODULES_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../Modules")
set(MODULES_LIB "${MODULES_ROOT}/lib")

include_directories(${MODULES_ROOT}/Utils)
include_directories(../Icarouslib/fsw/platform_inc)
include_directories(../scheduler/fsw/platform_inc)
include_directories(../traffic/fsw/platform_inc)
include_directories(../guidance/fsw/platform_inc)
include_directories(../geofence/fsw/platform_inc)
include_directories(../trajectory/fsw/platform_inc)
include_directories(../tracking/fsw/platform_inc)
include_directories(fsw/src)
include_directories(fsw/tables)
include_directories(../port_lib/fsw/public_inc)
aux_source_directory(fsw/src APP_SRC_FILES)


link_directories(${MODULES_LIB})

add_library(ardupilot MODULE ${APP_SRC_FILES})

set_target_properties(ardupilot PROPERTIES PREFIX "")


foreach(TGT ${APP_INSTALL_LIST})
    install(TARGETS ardupilot DESTINATION ${TGT}/${INSTALL_SUBDIR})
endforeach()

