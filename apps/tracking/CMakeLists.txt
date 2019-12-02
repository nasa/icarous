cmake_minimum_required(VERSION 2.6.4)
project(TRACKING_APP C)

set(CMAKE_CXX_STANDARD 11)

set(MODULES_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../Modules")
set(MODULES_LIB "${MODULES_ROOT}/lib")

include_directories(../Icarouslib/fsw/platform_inc)
include_directories(../scheduler/fsw/platform_inc)
include_directories(${MODULES_ROOT}/Utils)
include_directories(fsw/src)
include_directories(fsw/tables)
include_directories(fsw/platform_inc)

aux_source_directory(fsw/src APP_SRC_FILES)

link_directories(${MODULES_LIB})

add_library(tracking MODULE ${APP_SRC_FILES})

set_target_properties(tracking PROPERTIES PREFIX "")

#add_cfe_tables(tracking "fsw/tables/tracking_tbl.c")

foreach(TGT ${APP_INSTALL_LIST})
    install(TARGETS tracking DESTINATION ${TGT}/${INSTALL_SUBDIR})
endforeach()
