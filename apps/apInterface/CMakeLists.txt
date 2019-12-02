cmake_minimum_required(VERSION 2.6.4)
project(CFE_AP_INTERFACE C)

set(MODULES_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../../Modules")
set(MODULES_LIB "${MODULES_ROOT}/lib")

include_directories(fsw/mission_inc)
include_directories(fsw/platform_inc)
include_directories(../Icarouslib/fsw/platform_inc/)
include_directories(../scheduler/fsw/platform_inc/)
include_directories(../traffic/fsw/platform_inc/)
include_directories(fsw/src)
include_directories(fsw/tables)
include_directories(${MODULES_ROOT}/Utils)
include_directories(../port_lib/fsw/public_inc)

aux_source_directory(fsw/src APP_SRC_FILES)

link_directories(${MODULES_LIB})

add_library(apInterface MODULE ${APP_SRC_FILES})

set_target_properties(apInterface PROPERTIES PREFIX "")

target_link_libraries(apInterface Utils)
set_target_properties(apInterface PROPERTIES INSTALL_RPATH "${MODULES_LIB}")

foreach(TGT ${APP_INSTALL_LIST})
    install(TARGETS apInterface DESTINATION ${TGT}/${INSTALL_SUBDIR})
endforeach()
