cmake_minimum_required(VERSION 3.7)
project(universalExec)

set(CMAKE_CXX_STANDARD 11)

set(PLEXIL_ROOT ${CMAKE_SOURCE_DIR}/../)

set(PLEXIL_SOURCE ${PLEXIL_ROOT}/src)

set(CMAKE_CXX_FLAGS "-g -fPIC")

add_definitions(-DHAVE_CONFIG_H -DPIC)

set(LIBRARY_OUTPUT_PATH ${PLEXIL_ROOT}/lib)

set(SOURCE_FILES
        UniversalExec.cc)

include_directories(
        ./
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
        wrap nsl m rt pthread dl)
