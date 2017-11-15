cmake_minimum_required(VERSION 3.7)
project(PlexilAppFramework)

set(PLEXIL_SOURCE ${PLEXIL_ROOT}/src)

set(LIBRARY_OUTPUT_PATH ${PLEXIL_ROOT}/lib)

set(SOURCE_FILES
        AdapterConfiguration.cc
        AdapterConfiguration.hh
        AdapterExecInterface.cc
        AdapterExecInterface.hh
        AdapterFactory.cc
        AdapterFactory.hh
        DarwinTimeAdapter.cc
        DarwinTimeAdapter.hh
        DummyAdapter.cc
        DummyAdapter.hh
        ExecApplication.cc
        ExecApplication.hh
        ExecListener.cc
        ExecListener.hh
        ExecListenerFactory.cc
        ExecListenerFactory.hh
        ExecListenerFilter.cc
        ExecListenerFilter.hh
        ExecListenerFilterFactory.cc
        ExecListenerFilterFactory.hh
        ExecListenerHub.cc
        ExecListenerHub.hh
        InterfaceAdapter.cc
        InterfaceAdapter.hh
        InterfaceManager.cc
        InterfaceManager.hh
        ListenerFilters.cc
        ListenerFilters.hh
        PlexilListener.hh
        PosixTimeAdapter.cc
        PosixTimeAdapter.hh
        SerializedInputQueue.cc
        SerializedInputQueue.hh
        SimpleInputQueue.cc
        SimpleInputQueue.hh
        TimeAdapter.cc
        TimeAdapter.hh
        TimeAdapterImpl.cc
        TimeAdapterImpl.hh
        UtilityAdapter.cc
        UtilityAdapter.hh)

include_directories(
        ./
        ../
        ../utils
        ../value
        ../exec
        ../intfc
        ../expr
        ../third-party/pugixml/src
        ../xml-parser
        ../interfaces/PlanDebugListener
        ../interfaces/LuvListener
)

link_directories(${LIBRARY_OUTPUT_PATH})

add_library(PlexilAppFramework SHARED ${SOURCE_FILES})

target_link_libraries(PlexilAppFramework
        PlexilXmlParser
        PlexilExec
        PlexilIntfc
        PlexilExpr
        PlexilValue
        PlexilUtils
        pugixml
        wrap nsl m rt pthread dl)
