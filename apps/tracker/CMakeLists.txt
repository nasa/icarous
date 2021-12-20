cmake_minimum_required(VERSION 2.6.4)
project(TRACKER C)

set(CMAKE_CXX_STANDARD 11)

set(MODULES_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../Modules")
set(MODULES_LIB "${MODULES_ROOT}/lib")

include_directories(../Icarouslib/fsw/platform_inc)
include_directories(../scheduler/fsw/platform_inc)
include_directories(fsw/src)
include_directories(fsw/tables)
include_directories(${MODULES_ROOT}/Core/TargetTracker)
include_directories(${MODULES_ROOT}/Core/Interfaces)
include_directories(${MODULES_ROOT}/Core/Utils)
aux_source_directory(fsw/src APP_SRC_FILES)

link_directories(${MODULES_LIB})

add_library(tracker MODULE ${APP_SRC_FILES})

target_link_libraries(tracker TargetTracker Utils)

set_target_properties(tracker PROPERTIES PREFIX "")

set_target_properties(tracker PROPERTIES INSTALL_RPATH "${MODULES_LIB}")

foreach(TGT ${APP_INSTALL_LIST})
    install(TARGETS tracker DESTINATION ${TGT}/${INSTALL_SUBDIR})
endforeach()
