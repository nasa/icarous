cmake_minimum_required(VERSION 2.6.4)
project(RAFT_APP C)

set(CMAKE_CXX_STANDARD 11)

set(MODULES_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../Modules")
set(MODULES_LIB "${MODULES_ROOT}/lib")

include_directories(../inc)
include_directories(../merger/fsw/platform_inc)
include_directories(../scheduler/fsw/platform_inc)
include_directories(../Icarouslib/fsw/platform_inc)
include_directories(fsw/src)
include_directories(fsw/tables)
include_directories(fsw/platform_inc)

aux_source_directory(fsw/src APP_SRC_FILES)

link_directories(${MODULES_LIB})

add_library(raft MODULE ${APP_SRC_FILES})

set_target_properties(raft PROPERTIES PREFIX "")

set_target_properties(raft PROPERTIES INSTALL_RPATH "${MODULES_LIB}")

foreach(TGT ${APP_INSTALL_LIST})
    install(TARGETS raft DESTINATION ${TGT}/${INSTALL_SUBDIR})
endforeach()
