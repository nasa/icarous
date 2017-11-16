cmake_minimum_required(VERSION 2.6.4)
project(CFE_PLEXIL)

set(CMAKE_CXX_STANDARD 11)

set(PLEXIL_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/../../../C++/plexil-4/src")

set(PLEXIL_LIB "${PLEXIL_SOURCE}/../lib")

include_directories(fsw/mission_inc)
include_directories(fsw/platform_inc)
include_directories(fsw/src)

include_directories(
        ${PLEXIL_SOURCE}/interfaces/CfsAdapter
        ${PLEXIL_SOURCE}/wrapper
)

aux_source_directory(fsw/src APP_SRC_FILES)

set(SOURCE_FILES fsw/src/plexil.c)

link_directories(${PLEXIL_LIB})

add_library(plexil MODULE ${SOURCE_FILES})

target_link_libraries(plexil PlexilWrapper)

set_target_properties(plexil PROPERTIES PREFIX "")

set_target_properties(plexil PROPERTIES INSTALL_RPATH "${PLEXIL_LIB}")

foreach(TGT ${APP_INSTALL_LIST})
    install(TARGETS plexil DESTINATION ${TGT}/${INSTALL_SUBDIR})
endforeach()

add_cfe_tables(plexil "fsw/tables/plexil_tbl.c")
