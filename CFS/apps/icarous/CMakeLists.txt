cmake_minimum_required(VERSION 2.6.4)
project(CFE_icarous)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_STANDARD 11)

include_directories(fsw/mission_inc)
include_directories(fsw/mission_inc/ACCoRD)
include_directories(fsw/mission_inc/ICAROUS)
include_directories(fsw/platform_inc)
include_directories(fsw/src)

set(ICAROUS_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/../../../C++")

include_directories(
        ${ICAROUS_SOURCE}/include/ACCoRD
        ${ICAROUS_SOURCE}/include/ICAROUS
)


set(SOURCE_FILES fsw/src/icarous.c fsw/src/IcarousWrapper.cpp)

link_directories(${ICAROUS_SOURCE}/lib)

add_library(icarous MODULE ${SOURCE_FILES})

target_link_libraries(icarous ICAROUS ACCoRD)

set_target_properties(icarous PROPERTIES PREFIX "")

set_target_properties(icarous PROPERTIES INSTALL_RPATH "${ICAROUS_SOURCE}/lib")

foreach(TGT ${APP_INSTALL_LIST})
    install(TARGETS icarous DESTINATION ${TGT}/${INSTALL_SUBDIR})
endforeach()


add_cfe_tables(icarous "fsw/tables/icarous_tbl.c")