cmake_minimum_required(VERSION 2.6.4)
project(CFE_AP_COGNITION C)

set(MODULES_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../../Modules")
set(MODULES_LIB "${MODULES_ROOT}/lib")

include_directories(fsw/mission_inc)
include_directories(fsw/platform_inc)
include_directories(../icarous/fsw/platform_inc/)
include_directories(../inc)
include_directories(../inc/msgids)
include_directories(../inc/msgdef)
include_directories(fsw/src)
include_directories(fsw/tables)
include_directories(${MODULES_ROOT}/Utils)

aux_source_directory(fsw/src APP_SRC_FILES)

# Create the app module
add_cfe_app(cognition ${APP_SRC_FILES})

link_directories(${MODULES_LIB})
target_link_libraries(cognition Utils)
set_target_properties(cognition PROPERTIES INSTALL_RPATH "${MODULES_LIB}")
