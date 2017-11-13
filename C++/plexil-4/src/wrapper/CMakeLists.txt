cmake_minimum_required(VERSION 3.7)
project(Wrapper)

set(CMAKE_CXX_STANDARD 11)

set(PLEXIL_SOURCE ${PLEXIL_ROOT}/src)

set(CMAKE_CXX_FLAGS "-g -fPIC")

add_definitions(-DHAVE_CONFIG_H -DPIC)

set(LIBRARY_OUTPUT_PATH ${PLEXIL_ROOT}/lib)

set(SOURCE_FILES
        PlexilWrapper.h
        PlexilWrapper.cpp
        test.c)

include_directories(
        ./
        ${PLEXIL_SOURCE}
        ${PLEXIL_SOURCE}/utils
        ${PLEXIL_SOURCE}/value
        ${PLEXIL_SOURCE}/interfaces/Sockets
        ${PLEXIL_SOURCE}/interfaces/LuvListener
        ${PLEXIL_SOURCE}/interfaces/CfsAdapter
        ${PLEXIL_SOURCE}/third-party/pugixml/src
        ${PLEXIL_SOURCE}/app-framework
        ${PLEXIL_SOURCE}/expr
        ${PLEXIL_SOURCE}/intfc
        ${PLEXIL_SOURCE}/exec
)

link_directories(${LIBRARY_OUTPUT_PATH})

add_library(PlexilWrapper PlexilWrapper.cpp)

target_link_libraries(PlexilWrapper
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

add_executable(wrapperTest test.c)

target_link_libraries(wrapperTest PlexilWrapper)
