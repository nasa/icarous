cmake_minimum_required(VERSION 3.7)
project(LuvListener)

set(PLEXIL_SOURCE ${PLEXIL_ROOT}/src)

set(LIBRARY_OUTPUT_PATH ${PLEXIL_ROOT}/lib)

set(SOURCE_FILES
        LuvFormat.cc
        LuvFormat.hh
        LuvListener.cc
        LuvListener.hh)

include_directories(
        ./
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

add_library(LuvListener SHARED ${SOURCE_FILES})

target_link_libraries(LuvListener
        wrap nsl m rt pthread dl)