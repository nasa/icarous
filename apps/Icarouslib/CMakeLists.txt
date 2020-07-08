cmake_minimum_required(VERSION 2.6.4)
project(CFE_ICAROUS_LIB C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")

include_directories(fsw/public_inc)
include_directories(fsw/platform_inc)
include_directories(../traffic/fsw/platform_inc)
include_directories(../trajectory/fsw/platform_inc)
include_directories(../geofence/fsw/platform_inc)
include_directories(../tracking/fsw/platform_inc)
include_directories(../rotorsim/fsw/platform_inc)
include_directories(../merger/fsw/platform_inc)
include_directories(../guidance/fsw/platform_inc)
include_directories(../port_lib/fsw/public_inc)
include_directories(../../Modules/Interfaces)

aux_source_directory(fsw/src LIB_SRC_FILES)

# Create the app module
add_cfe_app(icarous_lib ${LIB_SRC_FILES})

