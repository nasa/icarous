cmake_minimum_required(VERSION 2.6)
project(universalExec)

set(PLEXIL_SOURCE ${PLEXIL_ROOT}/src)

set(LIBRARY_OUTPUT_PATH ${PLEXIL_ROOT}/lib)

set(SOURCE_FILES
        UniversalExec.cc)

include_directories(
        ./
        ${PLEXIL_ROOT}/include
        ${PLEXIL_SOURCE}
        ${PLEXIL_SOURCE}/utils
        ${PLEXIL_SOURCE}/value
        ${PLEXIL_SOURCE}/interfaces/Sockets
        ${PLEXIL_SOURCE}/interfaces/LuvListener
        ${PLEXIL_SOURCE}/third-party/pugixml/src
        ${PLEXIL_SOURCE}/app-framework
        ${PLEXIL_SOURCE}/expr
        ${PLEXIL_SOURCE}/intfc
        ${PLEXIL_SOURCE}/exec
)

link_directories(${LIBRARY_OUTPUT_PATH})

add_executable(universalExec ${SOURCE_FILES})

target_link_libraries(universalExec
        pugixml
        PlexilAppFramework
        PlexilExec
        PlexilIntfc
        PlexilExpr
        PlexilValue
        PlexilUtils
        PlanDebugListener
        LuvListener
        PlexilSockets
        CfsAdapter
        wrap nsl m rt pthread dl)
