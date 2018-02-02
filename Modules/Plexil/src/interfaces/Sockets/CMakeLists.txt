cmake_minimum_required(VERSION 3.5)
project(Sockets)

set(PLEXIL_SOURCE ${PLEXIL_ROOT}/src)

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
        ${PLEXIL_ROOT}/include
        ${PLEXIL_SOURCE}
        ${PLEXIL_SOURCE}/utils
)

link_directories(${LIBRARY_OUTPUT_PATH})

add_library(PlexilSockets SHARED ${SOURCE_FILES})

target_link_libraries(PlexilSockets
        wrap nsl m rt pthread dl)