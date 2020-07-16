cmake_minimum_required(VERSION 2.6)
project(PlexilIntfc)

set(LIBRARY_OUTPUT_PATH ${PLEXIL_ROOT}/lib)

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
        ${PLEXIL_ROOT}/include
        ../utils
        ../value
        ../expr
)

add_library(PlexilIntfc SHARED ${SOURCE_FILES})
