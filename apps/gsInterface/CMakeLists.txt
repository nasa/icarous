cmake_minimum_required(VERSION 2.6.4)
project(CFE_ARDUPILOT_GSINTERFACE C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")

set(MODULES_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../Modules")
set(MODULES_LIB "${MODULES_ROOT}/lib")



include_directories(../port_lib/fsw/public_inc)
include_directories(../Icarouslib/fsw/platform_inc)
include_directories(../scheduler/fsw/platform_inc)
include_directories(../traffic/fsw/platform_inc)
include_directories(../trajectory/fsw/platform_inc)
include_directories(../geofence/fsw/platform_inc)
include_directories(../tracking/fsw/platform_inc)
include_directories(../rotorsim/fsw/platform_inc)
include_directories(../merger/fsw/platform_inc)
include_directories(fsw/mission_inc)
include_directories(fsw/platform_inc)
include_directories(fsw/src)
include_directories(fsw/tables)
include_directories(${MODULES_ROOT}/Utils)

aux_source_directory(fsw/src APP_SRC_FILES)


link_directories(${MODULES_LIB})

add_library(gsInterface MODULE ${APP_SRC_FILES})

target_link_libraries(gsInterface Utils)

set_target_properties(gsInterface PROPERTIES PREFIX "")

set_target_properties(gsInterface PROPERTIES INSTALL_RPATH "${MODULES_LIB}")


foreach(TGT ${APP_INSTALL_LIST})
    install(TARGETS gsInterface DESTINATION ${TGT}/${INSTALL_SUBDIR})
endforeach()
