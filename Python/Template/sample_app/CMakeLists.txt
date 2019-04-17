cmake_minimum_required(VERSION 2.6.4)
project({{APP_NAMEU}} C)

set(CMAKE_CXX_STANDARD 11)

include_directories(../inc)
include_directories(fsw/src)
include_directories(fsw/tables)
aux_source_directory(fsw/src APP_SRC_FILES)

add_cfe_app({{APP_NAME}} ${APP_SRC_FILES})

#add_cfe_tables({{APP_NAME}} "fsw/tables/{{APP_NAME}}_tbl.c")

