cmake_minimum_required(VERSION 2.6.4)
project(CFE_PORT_LIB C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")

include_directories(fsw/public_inc)

# The shared OSAL and cFE include directories should always be used
# Note that this intentionally does NOT include PSP-specific includes, just the generic
include_directories(${CFECORE_SOURCE_DIR}/src/inc)
include_directories(${CFEPSP_SOURCE_DIR}/fsw/inc)
include_directories(fsw/public_inc)

aux_source_directory(fsw/src LIB_SRC_FILES)

# Create the app module
add_cfe_app(port_lib ${LIB_SRC_FILES})

