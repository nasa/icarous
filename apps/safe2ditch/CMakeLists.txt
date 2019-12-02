cmake_minimum_required(VERSION 2.6.4)
project(CFE_SAFE2DITCH C)

set(MODULES_LIB "${CMAKE_CURRENT_SOURCE_DIR}/../../../Modules/lib")

include_directories(../Icarouslib/fsw/platform_inc/)
include_directories(../port_lib/fsw/public_inc)
include_directories(fsw/mission_inc)
include_directories(fsw/platform_inc)
include_directories(fsw/src)
aux_source_directory(fsw/src APP_SRC_FILES)

# Create the app module
add_cfe_app(safe2ditch ${APP_SRC_FILES})

set_target_properties(safe2ditch PROPERTIES INSTALL_RPATH "${MODULES_LIB}")
