################################################################################
# Find OpenSplice tools:
# ----------------------
#
# PRECONDITIONS
#   * The environment variable $OSPL_HOME must set to your OpenSplice
#     installation directory.
#
# POSTCONDITIONS
#   * The following CMake variables will be set by this script:
#     - OpenSplice_FOUND        - TRUE if the installation had the correct
#                                 structure and the rest of the variables have
#                                 been assigned non-NOTFOUND values.
#     - OpenSplice_INCLUDE_DIRS - the OpenSplice include directories
#     - OpenSplice_LIBRARY_DIRS - the OpenSplice library directories
#     - OpenSplice_LIBRARIES    - the OpenSplice libraries to link
#     - OpenSplice_BINARY_DIR   - the OpenSplice binary directory
#     - OpenSplice_IDL_COMPILER - the OpenSplice binary for the IDL compiler
#
#   * The following CMake function will be available:
#
#       OpenSpliceCompileIdl(IDL_list_variable_name generated_files_list_variable_name)
#
#     that will set variable named ${generated_files_list_variable_name}
#
################################################################################

IF (NOT DEFINED ENV{OSPL_HOME})
    IF (OpenSplice_FIND_REQUIRED)
        message(FATAL_ERROR "$OSPL_HOME not defined but needed for DDS support")
    ELSE ()
        message(WARNING "Building without DDS support since $OSPL_HOME environment variable is not defined")
        return()
    ENDIF ()
ENDIF ()

IF (EXISTS $ENV{OSPL_HOME}
        AND EXISTS $ENV{OSPL_HOME}/include
        AND EXISTS $ENV{OSPL_HOME}/include/sys
        AND EXISTS $ENV{OSPL_HOME}/include/dcps/C++/isocpp
        AND EXISTS $ENV{OSPL_HOME}/include/dcps/C++/SACPP
        )
    SET(OpenSplice_INCLUDE_DIRS
            $ENV{OSPL_HOME}/include
            $ENV{OSPL_HOME}/include/sys
            $ENV{OSPL_HOME}/include/dcps/C++/isocpp
            $ENV{OSPL_HOME}/include/dcps/C++/SACPP
            )
ELSE ()
    MESSAGE(WARNING "OpenSplice include directories could not be found")
ENDIF ()

FIND_LIBRARY(KERNEL_LIBRARY
        NAMES
        ddskernel
        PATHS
        $ENV{OSPL_HOME}/lib
        )

FIND_LIBRARY(DDSOS_LIBRARY
        NAMES
        ddsos
        PATHS
        $ENV{OSPL_HOME}/lib
        )

FIND_LIBRARY(DDSDATABASE_LIBRARY
        NAMES
        ddsdatabase
        PATHS
        $ENV{OSPL_HOME}/lib
        )

FIND_LIBRARY(DCPSISOCPP_LIBRARY
        NAMES
        dcpsisocpp
        PATHS
        $ENV{OSPL_HOME}/lib
        )

FIND_LIBRARY(DCPSGAPI_LIBRARY
        NAMES
        dcpsgapi
        PATHS
        $ENV{OSPL_HOME}/lib
        )

FIND_LIBRARY(DCPSSACPP_LIBRARY
        NAMES
        dcpssacpp
        PATHS
        $ENV{OSPL_HOME}/lib
        )

IF (EXISTS ${KERNEL_LIBRARY}
        AND EXISTS ${DDSOS_LIBRARY}
        AND EXISTS ${DDSDATABASE_LIBRARY}
        AND EXISTS ${DCPSISOCPP_LIBRARY}
        AND EXISTS ${DCPSGAPI_LIBRARY}
        AND EXISTS ${DCPSSACPP_LIBRARY}
        )
    SET(OpenSplice_LIBRARIES
            ${KERNEL_LIBRARY}
            ${DDSOS_LIBRARY}
            ${DDSDATABASE_LIBRARY}
            ${DCPSISOCPP_LIBRARY}
            ${DCPSGAPI_LIBRARY}
            ${DCPSSACPP_LIBRARY}
            )
ELSE ()
    MESSAGE(WARNING "OpenSplice libraries could not be found")
ENDIF ()

FIND_PROGRAM(OpenSplice_IDL_COMPILER
        NAMES idlpp
        PATHS $ENV{OSPL_HOME}/bin
        DOC "OpenSplice IDL compiler"
        )

IF (EXISTS $ENV{OSPL_HOME}/lib)
    SET(OpenSplice_LIBRARY_DIR $ENV{OSPL_HOME}/lib)
ELSE ()
    MESSAGE(WARNING "OpenSplice library directory could not be found")
ENDIF ()

IF (EXISTS $ENV{OSPL_HOME}/bin)
    SET(OpenSplice_BINARY_DIR $ENV{OSPL_HOME}/bin)
ELSE ()
    MESSAGE(WARNING "OpenSplice binary directory could not be found")
ENDIF ()

IF (OpenSplice_INCLUDE_DIRS
        AND OpenSplice_LIBRARIES
        AND OpenSplice_IDL_COMPILER
        AND OpenSplice_LIBRARY_DIR
        )
    SET(OpenSplice_FOUND TRUE)
ENDIF ()

IF (OpenSplice_FOUND)
    MESSAGE(STATUS "OpenSplice installation found: $ENV{OSPL_HOME}")
ELSE ()
    IF (OpenSplice_FIND_REQUIRED)
        MESSAGE(FATAL_ERROR "Could not find a correct OpenSplice installation")
    ENDIF ()
ENDIF ()

MARK_AS_ADVANCED(
        OpenSplice_BINARY_DIR
        OpenSplice_IDL_COMPILER
)


###############################################################################
#
# OpenSpliceCompileIdl([idlFile] ...)
#
# Function that generates OpenSplice DDS source code from a list of IDL file.
#
###############################################################################

function(OpenSpliceCompileIdl idlFiles genfilesREF)

    function(getIDLOutputFilenames IDLfile API genfilesREF)
        GET_FILENAME_COMPONENT(it ${IDLfile} ABSOLUTE)
        GET_FILENAME_COMPONENT(nfile ${IDLfile} NAME_WE)
        GET_FILENAME_COMPONENT(pfile ${it} DIRECTORY)
        if (${API} STREQUAL "cpp")
            LIST(APPEND genfiles ${pfile}/gen/${nfile}.cpp ${pfile}/gen/${nfile}.h)
            LIST(APPEND genfiles ${pfile}/gen/${nfile}Dcps.cpp ${pfile}/gen/${nfile}Dcps.h)
            LIST(APPEND genfiles ${pfile}/gen/${nfile}Dcps_impl.cpp ${pfile}/gen/${nfile}Dcps_impl.h)
            LIST(APPEND genfiles ${pfile}/gen/${nfile}SplDcps.cpp ${pfile}/gen/${nfile}SplDcps.h)
            LIST(APPEND genfiles ${pfile}/gen/ccpp_${nfile}.h)
        else ()
            LIST(APPEND genfiles ${pfile}/gen/${nfile}.cpp ${pfile}/gen/${nfile}.h)
            LIST(APPEND genfiles ${pfile}/gen/${nfile}Dcps.cpp ${pfile}/gen/${nfile}Dcps.h)
            LIST(APPEND genfiles ${pfile}/gen/${nfile}Dcps_impl.cpp ${pfile}/gen/${nfile}Dcps_impl.h)
            LIST(APPEND genfiles ${pfile}/gen/${nfile}SplDcps.cpp ${pfile}/gen/${nfile}SplDcps.h)
            LIST(APPEND genfiles ${pfile}/gen/${nfile}_DCPS.hpp)
        endif ()
        set(${genfilesREF} ${genfiles} PARENT_SCOPE)
    endfunction()

    set(allgenfiles)
    set(API cpp)
    set(STANDALONE "-S")
    foreach (idlFile ${${idlFiles}})
        set(genfiles)
        GET_FILENAME_COMPONENT(absoluteFilename ${idlFile} ABSOLUTE)
        GET_FILENAME_COMPONENT(absoluteDirectory ${absoluteFilename} DIRECTORY)
        getIDLOutputFilenames(${idlFile} ${API} genfiles)
        LIST(APPEND allgenfiles ${genfiles})
        SET(OpenSplice_INCLUDE_DIR ${absoluteDirectory}/gen)
        INCLUDE_DIRECTORIES(${OpenSplice_INCLUDE_DIR})
        ADD_CUSTOM_COMMAND(
                OUTPUT ${genfiles}
                DEPENDS ${idlfilename}
                COMMAND
                LD_LIBRARY_PATH=${OpenSplice_LIBRARY_DIR}
                PATH=$ENV{PATH}:${OpenSplice_BINARY_DIR}
                ${OpenSplice_IDL_COMPILER}
                ARGS -l ${API} ${STANDALONE} -d ${OpenSplice_INCLUDE_DIR} ${absoluteFilename}
        )
    endforeach (idlFile)
    set(${genfilesREF} ${allgenfiles} PARENT_SCOPE)

endfunction()

