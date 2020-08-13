cmake_minimum_required(VERSION 2.6.4)
project(CFE_ARDUCOPTER_INTERFACE C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")

set(MODULES_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../Modules")
set(MODULES_LIB "${MODULES_ROOT}/lib")

include_directories(${MODULES_ROOT}/Core/Utils)
include_directories(${MODULES_ROOT}/Core/Interfaces)
include_directories(${MODULES_ROOT}/Core/Guidance)
include_directories(${MODULES_ROOT}/Core/TrajectoryManager/DubinsPlanner)

include_directories(../Icarouslib/fsw/platform_inc)
include_directories(../scheduler/fsw/platform_inc)
include_directories(../traffic/fsw/platform_inc)
include_directories(../guidance/fsw/platform_inc)
include_directories(../merger/fsw/platform_inc)
include_directories(../geofence/fsw/platform_inc)
include_directories(../trajectory/fsw/platform_inc)
include_directories(../tracking/fsw/platform_inc)
include_directories(../port_lib/fsw/public_inc)
include_directories(fsw/tables)
include_directories(fsw/src)

aux_source_directory(fsw/src APP_SRC_FILES)

link_directories(${MODULES_LIB})

add_library(arducopter MODULE ${APP_SRC_FILES})

target_link_libraries(arducopter Utils)

set_target_properties(arducopter PROPERTIES PREFIX "")

set_target_properties(arducopter PROPERTIES INSTALL_RPATH "${MODULES_LIB}")



foreach(TGT ${APP_INSTALL_LIST})
    install(TARGETS arducopter DESTINATION ${TGT}/${INSTALL_SUBDIR})
endforeach()

