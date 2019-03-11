#!/bin/bash

# ** OSAL PATCHES **
# Apply patches relevant to osal src files
patch -d ../cFS/cFE/osal/src/os/posix < osal_src_os_posix_osapi.patch
patch -d ../cFS/cFE/osal/src/os/posix < osal_src_os_posix_osfilesys.patch
patch -d ../cFS/cFE/osal/src/os/posix < osal_src_os_posix_osloader.patch
patch -d ../cFS/cFE/osal/src/os/posix < osal_src_os_posix_ostimer.patch

# Apply patch to osal inc files
patch -d ../cFS/cFE/osal/src/os/inc < osal_src_os_inc_osapi-os-loader.patch

# Apply patch to build options cmake
patch -d ../cFS/cFE/osal/src/os/posix < osal_src_os_posix_build_options.patch

# ** PSP PATCHES **
# Apply patches to psp src files
patch -d ../cFS/cFE/psp/fsw/pc-linux/src < cfe_psp_fsw_pc-linux_src_cfe_psp_memory.patch 

# ** CFE CORE PATCHES **
patch -d ../cFS/cFE/cfe/fsw/cfe-core/src/es < cfe_fsw_cfe-core_src_es_cfe_es_app.patch

patch -d ../cFS/cFE/cfe/cmake/target < cfe_cmake_target_cmklist.patch

# ** PATCH TOOL UTILITIES **
patch -d ../cFS/cFE/tools < tools_cmklist.patch


