cmake_minimum_required(VERSION 2.6.4)
project(CFE_AP_Functions)

include_directories(fsw/platform_inc)
include_directories(fsw/src)
include_directories(../inc)

set(SOURCE_FILES fsw/src/APFunctions.c)

add_library(APFunctions MODULE ${SOURCE_FILES})

set_target_properties(APFunctions PROPERTIES PREFIX "")

foreach(TGT ${APP_INSTALL_LIST})
    install(TARGETS APFunctions DESTINATION ${TGT}/${INSTALL_SUBDIR})
endforeach()