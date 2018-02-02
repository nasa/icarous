cmake_minimum_required(VERSION 2.6.4)
project(CFE_INTERFACE C)


set(PLEXIL_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/../../../Modules/Plexil/src")
set(PLEXIL_LIB "${PLEXIL_SOURCE}/../lib")
set(MODULES_LIB "${CMAKE_CURRENT_SOURCE_DIR}/../../../Modules/lib")

include_directories(fsw/mission_inc)
include_directories(fsw/platform_inc)
include_directories(../icarous/fsw/platform_inc/)
include_directories(../inc)
include_directories(fsw/src)
include_directories(${PLEXIL_SOURCE}/interfaces/CfsAdapter)
include_directories(${PLEXIL_SOURCE}/wrapper/)

aux_source_directory(fsw/src APP_SRC_FILES)

link_directories(${PLEXIL_LIB})

# Create the app module
add_cfe_app(interface ${APP_SRC_FILES})

target_link_libraries(interface PlexilWrapper)

set_target_properties(interface PROPERTIES INSTALL_RPATH "${PLEXIL_LIB}:${MODULES_LIB}")

add_cfe_tables(interface "fsw/tables/intf_tbl.c")
