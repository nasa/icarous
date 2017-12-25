cmake_minimum_required(VERSION 3.7)
project(PlanDebugListener)

set(PLEXIL_SOURCE ${PLEXIL_ROOT}/src)

set(LIBRARY_OUTPUT_PATH ${PLEXIL_ROOT}/lib)

set(SOURCE_FILES
        PlanDebugListener.cc
        PlanDebugListener.hh)

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

link_directories(${LIBRARY_OUTPUT_PATH})

add_library(PlanDebugListener SHARED ${SOURCE_FILES})

target_link_libraries(PlanDebugListener
        wrap nsl m rt pthread dl)