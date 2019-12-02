cmake_minimum_required(VERSION 2.6.4)
project(CFE_PLEXIL)

set(CMAKE_CXX_STANDARD 11)

set(PLEXIL_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/../../Modules/Plexil/src")
set(PLEXIL_LIB "${PLEXIL_SOURCE}/../lib")
set(MODULES_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../Modules")
set(MODULES_LIB "${CMAKE_CURRENT_SOURCE_DIR}/../../Modules/lib")
set(PLAN_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/plans")

include_directories(fsw/mission_inc)
include_directories(fsw/platform_inc)
include_directories(fsw/src)
include_directories(fsw/tables)
include_directories(../Icarouslib/fsw/platform_inc)
include_directories(../traffic/fsw/platform_inc)
include_directories(../trajectory/fsw/platform_inc)
include_directories(../geofence/fsw/platform_inc)
include_directories(../scheduler/fsw/platform_inc)
include_directories(../safe2ditch/fsw/platform_inc)
include_directories(../guidance/fsw/platform_inc)
include_directories(
        ${PLEXIL_SOURCE}/interfaces/CfsAdapter
        ${PLEXIL_SOURCE}/wrapper
)
include_directories(${MODULES_ROOT}/Utils)


aux_source_directory(fsw/src APP_SRC_FILES)

set(SOURCE_FILES fsw/src/plexil.c fsw/src/custom_functions.c)

link_directories(${PLEXIL_LIB})
link_directories(${MODULES_LIB})

add_library(plexil MODULE ${SOURCE_FILES})

target_link_libraries(plexil PlexilWrapper Utils)

set_target_properties(plexil PROPERTIES PREFIX "")

set_target_properties(plexil PROPERTIES INSTALL_RPATH "${PLEXIL_LIB}:${MODULES_LIB}")


foreach(TGT ${APP_INSTALL_LIST})
    install(TARGETS plexil DESTINATION ${TGT}/${INSTALL_SUBDIR})
endforeach()

#add_cfe_tables(plexil "fsw/tables/plexil_tbl.c")

set(PLEXIL_PLANS Icarous2.ple CONFLICT_MANAGEMENT.ple GOTO_POSITION.ple
           TRAFFIC_RESOLUTION.ple MONITOR.ple DITCH.ple)

#set(PLEXIL_PLANS ArdupilotTest.ple)


add_custom_target(PLEXIL_COMPILER COMMAND env PLEXIL_HOME=$ENV{PLEXIL_HOME} make plexil-compiler WORKING_DIRECTORY ${PLEXIL_SOURCE}/../ )
add_dependencies(plexil PLEXIL_COMPILER)

foreach(ARG ${PLEXIL_PLANS})
    #message(STATUS "Build plexil plan: ${ARG}" )
    add_custom_target(${ARG} COMMAND env PLEXIL_HOME=$ENV{PLEXIL_HOME} bash ${PLEXIL_SOURCE}/../scripts/plexilc ${ARG} WORKING_DIRECTORY ${PLAN_LOCATION})
    #if (NOT RESULT EQUAL 0)
    #    message(STATUS "Failed to compile plexil plan ${ARG}")
    #endif (NOT RESULT EQUAL 0)
    add_dependencies(${ARG} PLEXIL_COMPILER)
    add_dependencies(plexil ${ARG})
endforeach(ARG IN LISTS PLEXIL_PLANS)
