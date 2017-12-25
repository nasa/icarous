cmake_minimum_required(VERSION 3.7)
project(UdpAdapter)

set(PLEXIL_SOURCE ${PLEXIL_ROOT}/src)

set(LIBRARY_OUTPUT_PATH ${PLEXIL_ROOT}/lib)

set(SOURCE_FILES
        test/udp-tests.cc
        MessageQueueMap.cc
        MessageQueueMap.hh
        udp-utils.cc
        udp-utils.hh
        UdpAdapter.cc
        UdpAdapter.hh)

include_directories(
        ./
        ${PLEXIL_ROOT}/include
        ${PLEXIL_SOURCE}
        ${PLEXIL_SOURCE}/utils
        ${PLEXIL_SOURCE}/value
        ${PLEXIL_SOURCE}/interfaces/Sockets
        ${PLEXIL_SOURCE}/third-party/pugixml/src
        ${PLEXIL_SOURCE}/app-framework
        ${PLEXIL_SOURCE}/expr
        ${PLEXIL_SOURCE}/intfc
        ${PLEXIL_SOURCE}/exec
)

link_directories(${LIBRARY_OUTPUT_PATH})

add_library(UdpUtils SHARED udp-utils.cc)

add_library(UdpAdapter SHARED MessageQueueMap.cc UdpAdapter.cc)

target_link_libraries(UdpUtils
        wrap nsl m rt pthread dl)

target_link_libraries(UdpAdapter
        UdpUtils
        wrap nsl m rt pthread dl)