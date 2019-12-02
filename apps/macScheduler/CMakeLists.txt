cmake_minimum_required(VERSION 2.6.4)
project(CFE_MAC_SCHEDULER C)

include_directories(fsw/mission_inc)
include_directories(fsw/platform_inc)
include_directories(../scheduler/fsw/platform_inc/)
include_directories(fsw/src)
include_directories(fsw/tables)

aux_source_directory(fsw/src APP_SRC_FILES)

# Create the app module
add_cfe_app(macsch ${APP_SRC_FILES})

