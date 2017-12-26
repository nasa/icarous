cmake_minimum_required(VERSION 2.6.4)
project(CFE_INTERFACE C)

set(PLEXIL_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/../../../C++/Modules/Plexil/src")


include_directories(fsw/mission_inc)
include_directories(fsw/platform_inc)
include_directories(../icarous/fsw/platform_inc/)
include_directories(../inc)
include_directories(fsw/src)
include_directories(${PLEXIL_SOURCE}/interfaces/CfsAdapter)


aux_source_directory(fsw/src APP_SRC_FILES)

# Create the app module
add_cfe_app(interface ${APP_SRC_FILES})

add_cfe_tables(interface "fsw/tables/intf_tbl.c")
