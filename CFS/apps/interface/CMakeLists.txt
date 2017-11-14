cmake_minimum_required(VERSION 2.6.4)
project(CFE_INTERFACE C)

include_directories(fsw/mission_inc)
include_directories(fsw/platform_inc)
include_directories(../icarous/fsw/mission_inc/ICAROUS/)
include_directories(../icarous/fsw/platform_inc/)
include_directories(../inc)
include_directories(fsw/src)

aux_source_directory(fsw/src APP_SRC_FILES)

# Create the app module
add_cfe_app(interface ${APP_SRC_FILES})

add_cfe_tables(interface "fsw/tables/intf_tbl.c")
