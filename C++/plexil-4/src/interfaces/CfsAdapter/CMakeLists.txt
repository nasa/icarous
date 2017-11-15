cmake_minimum_required(VERSION 3.7)
project(CfsAdapter)

set(PLEXIL_SOURCE ${PLEXIL_ROOT}/src)

set(LIBRARY_OUTPUT_PATH ${PLEXIL_ROOT}/lib)

set(SOURCE_FILES
        CfsAdapter.cc
        CfsAdapter.hh
        cfs-data-format.hh
        )

include_directories(
        ./
        ${PLEXIL_SOURCE}
        ${PLEXIL_SOURCE}/utils
        ${PLEXIL_SOURCE}/value
        ${PLEXIL_SOURCE}/third-party/pugixml/src
        ${PLEXIL_SOURCE}/app-framework
        ${PLEXIL_SOURCE}/xml-parser
        ${PLEXIL_SOURCE}/expr
        ${PLEXIL_SOURCE}/intfc
        ${PLEXIL_SOURCE}/exec
        ${PLEXIL_SOURCE}/interfaces/IpcUtils
)

link_directories(${LIBRARY_OUTPUT_PATH})

add_library(CfsAdapter SHARED ${SOURCE_FILES})

target_link_libraries(CfsAdapter
        PlexilValue
        PlexilUtils
        wrap nsl m rt pthread dl)