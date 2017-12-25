cmake_minimum_required(VERSION 3.7)
project(GanttListener)

set(PLEXIL_SOURCE ${PLEXIL_ROOT}/src)

set(LIBRARY_OUTPUT_PATH ${PLEXIL_ROOT}/lib)


set(SOURCE_FILES
        GanttListener.cc
        GanttListener.hh)

include_directories(
        ./
        ${PLEXIL_ROOT}/include
        ${PLEXIL_SOURCE}
        ${PLEXIL_SOURCE}/utils
        ${PLEXIL_SOURCE}/value
        ${PLEXIL_SOURCE}/third-party/pugixml/src
        ${PLEXIL_SOURCE}/app-framework
        ${PLEXIL_SOURCE}/expr
        ${PLEXIL_SOURCE}/intfc
        ${PLEXIL_SOURCE}/exec
)

link_directories(LIBRARY_OUTPUT_PATH)

add_library(GanttListener SHARED ${SOURCE_FILES})

target_link_libraries(GanttListener wrap nsl m rt pthread dl)