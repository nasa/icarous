cmake_minimum_required(VERSION 2.6.4)
project(GUIDANCE C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")

set(CMAKE_CXX_STANDARD 11)

include_directories(fsw/src)
include_directories(fsw/tables)
include_directories(../Icarouslib/fsw/platform_inc)
include_directories(../scheduler/fsw/platform_inc)
include_directories(../trajectory/fsw/platform_inc)
include_directories(../guidance/fsw/platform_inc)


set(MODULES_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../Modules")
set(MODULES_LIB "${MODULES_ROOT}/lib")
include_directories(${MODULES_ROOT}/Utils)
include_directories(${MODULES_ROOT}/Guidance)

aux_source_directory(fsw/src APP_SRC_FILES)

link_directories(${MODULES_LIB})

add_library(guidance MODULE ${APP_SRC_FILES})

target_link_libraries(guidance Utils Guidance)

set_target_properties(guidance PROPERTIES PREFIX "")

set_target_properties(guidance PROPERTIES INSTALL_RPATH "${MODULES_LIB}")

foreach(TGT ${APP_INSTALL_LIST})
    install(TARGETS guidance DESTINATION ${TGT}/${INSTALL_SUBDIR})
endforeach()



#add_cfe_tables(Guidance "fsw/tables/Guidance_tbl.c")
