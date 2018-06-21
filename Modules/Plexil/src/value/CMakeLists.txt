cmake_minimum_required(VERSION 2.6)
project(PlexilValue)


set(PLEXIL_SOURCE ${PLEXIL_ROOT}/src)

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
        ${PLEXIL_ROOT}/include
        ../utils
)

add_library(PlexilValue SHARED ${SOURCE_FILES})

target_link_libraries(PlexilValue wrap nsl rt pthread dl)