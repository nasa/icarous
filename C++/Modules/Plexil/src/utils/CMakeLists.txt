cmake_minimum_required(VERSION 3.5)
project(PlexilUtils)

set(LIBRARY_OUTPUT_PATH ${PLEXIL_ROOT}/lib)

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

if(DEFINED ENV{JAVA_HOME})
    add_library(PlexilUtils SHARED ${SOURCE_FILES})

    include_directories(
            $ENV{JAVA_HOME}/include/
            $ENV{JAVA_HOME}/include/linux/
            ${PLEXIL_ROOT}/include)

else()
    message(FATAL_ERROR "JAVA_HOME not defined.")
endif()




target_link_libraries(PlexilUtils wrap nsl m rt pthread dl)