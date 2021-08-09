cmake_minimum_required(VERSION 2.6.4)
project(CFE_FLARM C)

set(MODULES_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../Modules")
set(MODULES_LIB "${MODULES_ROOT}/lib")

include_directories(fsw/mission_inc)
include_directories(fsw/platform_inc)
include_directories(fsw/src)
include_directories(fsw/tables)
include_directories(../Icarouslib/fsw/platform_inc/)
include_directories(../port_lib/fsw/public_inc)
include_directories(${MODULES_ROOT}/Core/Utils)
include_directories(${MODULES_ROOT}/Core/Interfaces)

aux_source_directory(fsw/src APP_SRC_FILES)

link_directories(${MODULES_LIB})
#link_directories(${MODULES_LIB}/../../cFS/bin/cpu1/cf)


# Create the app module
add_cfe_app(flarm ${APP_SRC_FILES})

target_link_libraries(flarm Utils)
set_target_properties(flarm PROPERTIES INSTALL_RPATH "${MODULES_LIB}")
