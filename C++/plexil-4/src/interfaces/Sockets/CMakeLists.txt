cmake_minimum_required(VERSION 3.7)
project(Sockets)

set(CMAKE_CXX_STANDARD 11)

set(PLEXIL_ROOT ${CMAKE_SOURCE_DIR}/../../../)

set(PLEXIL_SOURCE ${PLEXIL_ROOT}/src)

set(CMAKE_CXX_FLAGS "-O2 -fPIC")

add_definitions(-DHAVE_CONFIG_H -DPIC)

set(LIBRARY_OUTPUT_PATH ${PLEXIL_ROOT}/lib)

set(SOURCE_FILES
        ClientSocket.cpp
        ClientSocket.h
        ServerSocket.cpp
        ServerSocket.h
        Socket.cpp
        Socket.h
        SocketException.h)

include_directories(
        ./
        ${PLEXIL_SOURCE}
        ${PLEXIL_SOURCE}/utils
)

link_directories(${LIBRARY_OUTPUT_PATH})

add_library(PlexilSockets SHARED ${SOURCE_FILES})

target_link_libraries(PlexilSockets
        wrap nsl m rt pthread dl)