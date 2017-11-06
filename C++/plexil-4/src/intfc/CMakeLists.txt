cmake_minimum_required(VERSION 3.7)
project(PlexilIntfc)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "-O2 -fPIC")

add_definitions(-DHAVE_CONFIG_H -DPIC)

set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}../../../lib)

set(SOURCE_FILES
        CachedValue.cc
        CachedValue.hh
        CachedValueImpl.cc
        CachedValueImpl.hh
        Command.cc
        Command.hh
        CommandHandleVariable.cc
        CommandHandleVariable.hh
        ExprVec.cc
        ExprVec.hh
        ExternalInterface.cc
        ExternalInterface.hh
        InputQueue.hh
        InterfaceError.cc
        InterfaceError.hh
        Lookup.cc
        Lookup.hh
        QueueEntry.cc
        QueueEntry.hh
        resource-tags.cc
        resource-tags.hh
        ResourceArbiterInterface.cc
        ResourceArbiterInterface.hh
        State.cc
        State.hh
        StateCacheEntry.cc
        StateCacheEntry.hh
        StateCacheMap.cc
        StateCacheMap.hh
        Update.cc
        Update.hh)

include_directories(
        ./
        ../
        ../utils
        ../value
        ../expr
)

add_library(PlexilIntfc SHARED ${SOURCE_FILES})

target_link_libraries(PlexilIntfc wrap nsl m rt pthread dl)
