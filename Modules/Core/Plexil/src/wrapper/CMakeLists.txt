cmake_minimum_required(VERSION 2.6)
project(Wrapper)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")
set(PLEXIL_SOURCE ${PLEXIL_ROOT}/src)

set(LIBRARY_OUTPUT_PATH ${PLEXIL_ROOT}/lib)

set(SOURCE_FILES
        PlexilWrapper.h
        Serialize.h
        PlexilWrapper.cpp
        test.c)

include_directories(
        ./
        ${PLEXIL_ROOT}/include
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
        CfsAdapter)

target_link_libraries(PlexilWrapper m dl pthread)

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
  target_link_libraries(PlexilWrapper nsl rt)
else()
endif()

add_executable(wrapperTest test.c)

target_link_libraries(wrapperTest PlexilWrapper)
