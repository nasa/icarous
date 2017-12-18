cmake_minimum_required(VERSION 2.6.4)
project(icarouslib)

include_directories(fsw/mission_inc)
include_directories(fsw/mission_inc/ICAROUS)
include_directories(fsw/mission_inc/ACCoRD)
include_directories(fsw/platform_inc)
include_directories(../icarous/fsw/src)

aux_source_directory(fsw/src APP_SRC_FILES)

# Create the app module
add_cfe_app(icarouslib ${APP_SRC_FILES})
