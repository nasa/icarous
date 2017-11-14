//
// Created by Swee Balachandran on 11/13/17.
//

#ifndef CFETOP_PLEXIL_H
#define CFETOP_PLEXIL_H

/// Required header files.
#include "cfe.h"
#include "cfe_error.h"
#include "cfe_evs.h"
#include "cfe_sb.h"
#include "cfe_es.h"

#include <string.h>
#include <errno.h>
#include "PlexilWrapper.h"

#define PLEXIL_MAJOR_VERSION 4
#define PLEXIL_MINOR_VERSION 0

/// Software bus properties
#define PLEXIL_PIPE_NAME "PLEXILPIPE"
#define PLEXIL_PIPE_DEPTH 32

// Plexil App event defines

#define PLEXIL_RESERVED_EID              0
#define PLEXIL_STARTUP_INF_EID           1
#define PLEXIL_COMMAND_ERR_EID           2
#define PLEXIL_INVALID_MSGID_ERR_EID     3


// Plexil Message types

#define PLEXIL_RETURN_MID 1
#define PLEXIL_COMMAND_MID 2

EXTERN plexilAppData_t plexilAppdata;                ///< global variable containing app state
EXTERN PlexilCommandMsg plexilMsg;

/**
 * \struct plexilAppData
 * Structure to hold app data
 */
typedef struct{
    CFE_SB_PipeId_t    PLEXIL_Pipe;       ///< pipe variable
    CFE_SB_MsgPtr_t    PLEXIL_MsgPtr;     ///< msg pointer to SB message
    CFE_TBL_Handle_t   PLEXIL_tblHandle;  ///< table handle
    struct plexilExec* exec;
    struct plexilInterfaceAdapter* adap;
}plexilAppData_t;

typedef struct
{
    int argc;
    char argv[250];

}PlexilTable_t;


#endif //CFETOP_PLEXIL_H
