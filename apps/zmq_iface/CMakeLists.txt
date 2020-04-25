cmake_minimum_required(VERSION 2.6.4)
project(ZMQ_IFACE C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)

set(MODULES_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../Modules")

include_directories(../inc)
include_directories(../Icarouslib/fsw/platform_inc)
include_directories(../traffic/fsw/platform_inc)
include_directories(${MODULES_ROOT}/Core/Interfaces)
include_directories(fsw/src)
aux_source_directory(fsw/src APP_SRC_FILES)

add_cfe_app(zmq_iface ${APP_SRC_FILES})
target_link_libraries(zmq_iface zmq json-c)

