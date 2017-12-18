cmake_minimum_required(VERSION 2.6.4)
project(CFE_Icarous_APP C)

include_directories(fsw/mission_inc)
include_directories(fsw/platform_inc)
include_directories(fsw/src)
include_directories(../icarouslib/fsw/mission_inc)
include_directories(../icarouslib/fsw/mission_inc/ICAROUS)

aux_source_directory(fsw/src APP_SRC_FILES)

# Create the app module
add_cfe_app(icarous ${APP_SRC_FILES})

add_cfe_tables(icarous "fsw/tables/icarous_tbl.c")
