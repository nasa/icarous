cmake_minimum_required(VERSION 2.6.4)
project(MERGER_APP C)

set(CMAKE_CXX_STANDARD 11)

set(MODULES_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../../Modules")
set(MODULES_LIB "${MODULES_ROOT}/lib")

include_directories(../inc)
include_directories(fsw/src)
include_directories(fsw/tables)
include_directories(${MODULES_ROOT}/Utils)

aux_source_directory(fsw/src APP_SRC_FILES)

link_directories(${MODULES_LIB})

add_library(merger MODULE ${APP_SRC_FILES})

target_link_libraries(merger Utils)

set_target_properties(merger PROPERTIES PREFIX "")

set_target_properties(merger PROPERTIES INSTALL_RPATH "${MODULES_LIB}")

foreach(TGT ${APP_INSTALL_LIST})
    install(TARGETS merger DESTINATION ${TGT}/${INSTALL_SUBDIR})
endforeach()

#add_cfe_tables(merger "fsw/tables/merger_tbl.c")
