cmake_minimum_required(VERSION 3.7)
project(PlexilValue)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "-O2 -fPIC")

add_definitions(-DHAVE_CONFIG_H -DPIC)

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