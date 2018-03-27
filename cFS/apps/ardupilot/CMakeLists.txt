cmake_minimum_required(VERSION 2.6.4)
project(CFE_ARDUPILOT_INTERFACE C)

set(MODULES_LIB "${CMAKE_CURRENT_SOURCE_DIR}/../../../Modules/lib")

include_directories(fsw/mission_inc)
include_directories(fsw/platform_inc)
include_directories(../icarous/fsw/platform_inc/)
include_directories(../inc)
include_directories(fsw/src)

aux_source_directory(fsw/src APP_SRC_FILES)

link_directories(${PLEXIL_LIB})

# Create the app module
add_cfe_app(ardupilot ${APP_SRC_FILES})

target_link_libraries(ardupilot PlexilWrapper)

set_target_properties(ardupilot PROPERTIES INSTALL_RPATH "${PLEXIL_LIB}:${MODULES_LIB}")

add_cfe_tables(ardupilot "fsw/tables/intf_tbl.c")
