cmake_minimum_required(VERSION 3.7)
project(PlexilValue)

set(PLEXIL_ROOT ${CMAKE_SOURCE_DIR}/../../)

set(PLEXIL_SOURCE ${PLEXIL_ROOT}/src)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "-O2 -fPIC")

add_definitions(-DHAVE_CONFIG_H -DPIC)

set(LIBRARY_OUTPUT_PATH ${PLEXIL_ROOT}/lib)

set(SOURCE_FILES
        Array.cc
        Array.hh
        ArrayFwd.hh
        ArrayImpl.cc
        ArrayImpl.hh
        CommandHandle.cc
        CommandHandle.hh
        NodeConstants.cc
        NodeConstants.hh
        PlexilTypeTraits.cc
        PlexilTypeTraits.hh
        Value.cc
        Value.hh
        ValueType.cc
        ValueType.hh)

include_directories(
        ../
        ../utils
)

add_library(PlexilValue SHARED ${SOURCE_FILES})

target_link_libraries(PlexilValue wrap nsl rt pthread dl)