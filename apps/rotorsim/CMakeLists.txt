cmake_minimum_required(VERSION 2.6.4)
project(ROTORSIM_APP C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")

set(CMAKE_CXX_STANDARD 11)

set(MODULES_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../Modules")
set(MODULES_LIB "${MODULES_ROOT}/lib")

include_directories(../Icarouslib/fsw/platform_inc)
include_directories(../scheduler/fsw/platform_inc)
include_directories(${MODULES_ROOT}/Utils)
include_directories(fsw/src)
include_directories(fsw/tables)
include_directories(QuadCopterSim)

aux_source_directory(fsw/src APP_SRC_FILES)

aux_source_directory(QuadCopterSim SIM_SRC_FILES)

link_directories(${MODULES_LIB})

add_library(rotorsim MODULE ${APP_SRC_FILES} ${SIM_SRC_FILES})

target_link_libraries(rotorsim Utils)

set_target_properties(rotorsim PROPERTIES PREFIX "")

set_target_properties(rotorsim PROPERTIES INSTALL_RPATH "${MODULES_LIB}")


foreach(TGT ${APP_INSTALL_LIST})
    install(TARGETS rotorsim DESTINATION ${TGT}/${INSTALL_SUBDIR})
endforeach()
