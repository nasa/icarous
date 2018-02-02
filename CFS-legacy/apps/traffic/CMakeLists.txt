cmake_minimum_required(VERSION 2.6.4)
project(TRAFFIC_APP C)

set(CMAKE_CXX_STANDARD 11)

set(MODULES_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../../C++/Modules")
set(MODULES_LIB "${CMAKE_CURRENT_SOURCE_DIR}/../../../C++/lib")
set(PLEXIL_SOURCE "${MODULES_ROOT}/Plexil/src")
set(PLEXIL_LIB "${PLEXIL_SOURCE}/../lib")

include_directories(../inc)
include_directories(${MODULES_ROOT}/TrafficMonitor)
include_directories(${MODULES_ROOT}/TrafficMonitor/CWrapper)
include_directories(${PLEXIL_SOURCE}/interfaces/CfsAdapter)
include_directories(${PLEXIL_SOURCE}/wrapper)

aux_source_directory(fsw/src APP_SRC_FILES)

link_directories(${MODULES_LIB})
link_directories(${PLEXIL_LIB})

add_library(traffic MODULE ${APP_SRC_FILES} ${MODULES_ROOT}/TrafficMonitor/CWrapper/TrafficMonitor_proxy.c fsw/src/TrafficPlxMsgHandler.c)

target_link_libraries(traffic TrafficMonitor PlexilWrapper)

set_target_properties(traffic PROPERTIES PREFIX "")

set_target_properties(traffic PROPERTIES INSTALL_RPATH "${MODULES_LIB}:${PLEXIL_LIB}")

add_dependencies(traffic TrafficMonitor-all Plexil-all)

foreach(TGT ${APP_INSTALL_LIST})
    install(TARGETS traffic DESTINATION ${TGT}/${INSTALL_SUBDIR})
endforeach()
