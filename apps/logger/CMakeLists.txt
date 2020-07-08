cmake_minimum_required(VERSION 2.6.4)
project(CFE_LOGGER C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")

include_directories(../Icarouslib/fsw/platform_inc)
include_directories(fsw/src)
include_directories(fsw/tables)

aux_source_directory(fsw/src APP_SRC_FILES)

link_directories(${MODULES_LIB})

add_library(logger MODULE ${APP_SRC_FILES})

set_target_properties(logger PROPERTIES PREFIX "")

foreach(TGT ${APP_INSTALL_LIST})
    install(TARGETS logger DESTINATION ${TGT}/${INSTALL_SUBDIR})
endforeach()

