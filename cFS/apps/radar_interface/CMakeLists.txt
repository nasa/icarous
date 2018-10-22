cmake_minimum_required(VERSION 2.6.4)
project(CFE_RADAR_INTERFACE C)

set(MODULES_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../../Modules")
set(MODULES_LIB "${CMAKE_CURRENT_SOURCE_DIR}/../../../Modules/lib")

include_directories(fsw/mission_inc)
include_directories(fsw/platform_inc)
include_directories(../icarous/fsw/platform_inc/)
include_directories(../inc/msgdef)
include_directories(../inc/msgids)
include_directories(fsw/src)
include_directories(../ardupilot/fsw/mission_inc/)
include_directories(${MODULES_ROOT}/Utils)

aux_source_directory(fsw/src APP_SRC_FILES)

link_directories(${MODULES_LIB})

add_library(radarinterface MODULE ${APP_SRC_FILES})

target_link_libraries(radarinterface Utils)

set_target_properties(radarinterface PROPERTIES PREFIX "")

set_target_properties(radarinterface PROPERTIES INSTALL_RPATH "${MODULES_LIB}")

foreach(TGT ${APP_INSTALL_LIST})
    install(TARGETS radarinterface DESTINATION ${TGT}/${INSTALL_SUBDIR})
endforeach()

