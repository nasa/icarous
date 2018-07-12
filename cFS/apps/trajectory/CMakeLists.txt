cmake_minimum_required(VERSION 2.6.4)
project(TRAJECTORY_APP C)

set(CMAKE_CXX_STANDARD 11)

set(MODULES_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../../Modules")
set(MODULES_LIB "${MODULES_ROOT}/lib")

include_directories(../inc/msgdef)
include_directories(../inc/msgids)
include_directories(fsw/src)
include_directories(${MODULES_ROOT}/Utils)
include_directories(${MODULES_ROOT}/TrajectoryPlanners)
include_directories(${MODULES_ROOT}/TrajectoryPlanners/CWrapper)
include_directories(${PLEXIL_SOURCE}/interfaces/CfsAdapter)
include_directories(${PLEXIL_SOURCE}/wrapper)

aux_source_directory(fsw/src APP_SRC_FILES)

link_directories(${MODULES_LIB})

add_library(trajectory MODULE ${APP_SRC_FILES} ${MODULES_ROOT}/TrajectoryPlanners/CWrapper/TrajectoryPlanner_proxy.c)

target_link_libraries(trajectory PathPlanner)

set_target_properties(trajectory PROPERTIES PREFIX "")

set_target_properties(trajectory PROPERTIES INSTALL_RPATH "${MODULES_LIB}")

add_cfe_tables(trajectory "fsw/tables/trajectory_tbl.c")

foreach(TGT ${APP_INSTALL_LIST})
    install(TARGETS trajectory DESTINATION ${TGT}/${INSTALL_SUBDIR})
endforeach()
