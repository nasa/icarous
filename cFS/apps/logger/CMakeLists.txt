cmake_minimum_required(VERSION 2.6.4)
project(CFE_LOGGER C)

include_directories(../inc)
include_directories(../inc/msgids)
include_directories(../inc/msgdef)
include_directories(fsw/src)

aux_source_directory(fsw/src APP_SRC_FILES)

# Create the app module
add_cfe_app(logger ${APP_SRC_FILES})

add_cfe_tables(logger "fsw/tables/logger_tbl.c")
