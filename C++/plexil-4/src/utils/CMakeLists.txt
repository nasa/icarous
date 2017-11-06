cmake_minimum_required(VERSION 3.7)
project(PlexilUtils)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "-O2 -fPIC")

add_definitions(-DHAVE_CONFIG_H -DPIC)

set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/../lib)


set(SOURCE_FILES
        bitsetUtils.cc
        bitsetUtils.hh
        ConstantMacros.hh
        Debug.hh
        DebugMessage.cc
        DebugMessage.hh
        DynamicLoader.cc
        DynamicLoader.h
        Error.cc
        Error.hh
        JNIHelper.cc
        JNIHelper.hh
        JNIUtils.cc
        JNIUtils.hh
        lifecycle-utils.c
        lifecycle-utils.h
        LinkedQueue.hh
        Logging.cc
        Logging.hh
        map-utils.hh
        ParserException.cc
        ParserException.hh
        PlanError.cc
        PlanError.hh
        RecursiveThreadMutex.cc
        RecursiveThreadMutex.hh
        ScopedOstreamRedirect.hh
        SimpleMap.hh
        stricmp.c
        stricmp.h
        TestSupport.hh
        ThreadMutex.cc
        ThreadMutex.hh
        ThreadSemaphore.cc
        ThreadSemaphore.hh
        ThreadSpawn.cc
        ThreadSpawn.hh
        timespec-utils.cc
        timespec-utils.hh
        timeval-utils.cc
        timeval-utils.hh
        utils_main_page.hh)

include_directories(../
        /usr/lib/jvm/java-8-openjdk-amd64/include/)

add_library(PlexilUtils SHARED ${SOURCE_FILES})

target_link_libraries(PlexilUtils wrap nsl m rt pthread dl)