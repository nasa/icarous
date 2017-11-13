cmake_minimum_required(VERSION 3.7)
project(IpcUtils)

set(CMAKE_CXX_STANDARD 11)

set(PLEXIL_SOURCE ${PLEXIL_ROOT}/src)

set(CMAKE_CXX_FLAGS "-g -fPIC")

add_definitions(-DHAVE_CONFIG_H -DPIC)

set(LIBRARY_OUTPUT_PATH ${PLEXIL_ROOT}/../lib)


set(SOURCE_FILES
        ipc-data-formats.h
        IpcFacade.cc
        IpcFacade.hh)

include_directories(
        ./
        ${PLEXIL_SOURCE}
        ${PLEXIL_SOURCE}/utils
        ${PLEXIL_SOURCE}/value
        ${PLEXIL_SOURCE}/third-party/ipc/src
        ${PLEXIL_SOURCE}/third-party/ooid/kashmir
)

link_directories(${PLEXIL_ROOT}/lib)

add_library(IpcUtils SHARED ${SOURCE_FILES})

target_link_libraries(IpcUtils
        PlexilValue
        PlexilUtils
        ipc
        wrap nsl m rt pthread dl)