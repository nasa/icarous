cmake_minimum_required(VERSION 2.6.4)
project(CFE_DDS_INTERFACE C)


include_directories(fsw/mission_inc)
include_directories(fsw/platform_inc)
include_directories(../icarous/fsw/platform_inc/)
include_directories(../inc)
include_directories(fsw/src)

aux_source_directory(fsw/src APP_SRC_FILES)


# Create the app module
add_cfe_app(dds_interface ${APP_SRC_FILES})

find_package(OpenSplice REQUIRED)
MESSAGE(STATUS "DDS: compiling with OpenSplice support")
include_directories(
        ./fsw/src/extra/
        ./fsw/src/extra/gen
        ${OpenSplice_INCLUDE_DIRS}
)

add_definitions(-DWITH_DDS)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -std=c++11")

set(IDL_FILES ./fsw/src/extra/GPSInfo.idl)
OpenSpliceCompileIdl(IDL_FILES SB_DDS_FILES)

add_library(DDSlib
        fsw/src/extra/dds.cpp
        ${SB_DDS_FILES}
        )

target_link_libraries(dds_interface
        DDSlib
        ${OpenSplice_LIBRARIES}
        dl
        )

