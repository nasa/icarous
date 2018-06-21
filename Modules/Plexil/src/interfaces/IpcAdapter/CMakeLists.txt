cmake_minimum_required(VERSION 2.6)
project(IpcAdapter)

set(PLEXIL_SOURCE ${PLEXIL_ROOT}/src)

set(LIBRARY_OUTPUT_PATH ${PLEXIL_ROOT}/lib)

set(SOURCE_FILES
        initIpcAdapter.cc
        IpcAdapter.cc
        IpcAdapter.hh
        MessageQueueMap.cc
        MessageQueueMap.hh)

include_directories(
        ./
        ${PLEXIL_ROOT}/include
        ${PLEXIL_SOURCE}
        ${PLEXIL_SOURCE}/utils
        ${PLEXIL_SOURCE}/value
        ${PLEXIL_SOURCE}/third-party/pugixml/src
        ${PLEXIL_SOURCE}/third-party/ipc/src
        ${PLEXIL_SOURCE}/app-framework
        ${PLEXIL_SOURCE}/xml-parser
        ${PLEXIL_SOURCE}/expr
        ${PLEXIL_SOURCE}/intfc
        ${PLEXIL_SOURCE}/exec
        ${PLEXIL_SOURCE}/interfaces/IpcUtils
)

link_directories(${LIBRARY_OUTPUT_PATH})

add_library(IpcAdapter SHARED ${SOURCE_FILES})

target_link_libraries(IpcAdapter
        IpcUtils
        wrap nsl m rt pthread dl)