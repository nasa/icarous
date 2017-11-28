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
#include "FlightDataWrapper.h"
#include "Plexil_msg.h"

#define PLEXIL_MAJOR_VERSION 4
#define PLEXIL_MINOR_VERSION 0

/// Software bus properties
#define PLEXIL_PIPE_NAME "PLEXILPIPE"
#define PLEXIL_PIPE_DEPTH 32

/// Software bus properties
#define FLIGHTDATA_PIPE_NAME "FLIGHTDATAPIPE"
#define FLIGHTDATA_PIPE_DEPTH 32

// plexil App event defines

#define PLEXIL_RESERVED_EID              0
#define PLEXIL_STARTUP_INF_EID           1
#define PLEXIL_COMMAND_ERR_EID           2
#define PLEXIL_INVALID_MSGID_ERR_EID     3

/// Defines required to specify stack properties
#define TASK_PLDAQ_ID         38
#define TASK_PLDAQ_STACK_SIZE 1024
#define TASK_PLDAQ_PRIORITY   68

uint32 task_pldaq_stack[TASK_PLDAQ_STACK_SIZE];
uint32 task_pldaq_id;


/**
 * \struct plexilAppData
 * Structure to hold app data
 */
typedef struct{
    CFE_SB_PipeId_t    PLEXIL_Pipe;       ///< pipe variable
    CFE_SB_PipeId_t    FlightData_Pipe;       ///< pipe variable
    CFE_SB_MsgPtr_t    PLEXIL_MsgPtr;     ///< msg pointer to SB message
    CFE_SB_MsgPtr_t    FlightData_MsgPtr;     ///< msg pointer to SB message
    CFE_TBL_Handle_t   PLEXIL_tblHandle;  ///< table handle
    struct plexilExec* exec;
    struct plexilInterfaceAdapter* adap;
    struct flightData* fData;
    uint32_t mutex;
    uint32_t threadState;
}plexilAppData_t;

typedef struct
{
    int argc;
    char argv1[50];
    char argv2[50];
    char argv3[50];
    char argv4[50];
    char argv5[50];
    char argv6[50];
    char argv7[50];

}PLEXILTable_t;


EXTERN plexilAppData_t plexilAppData;                ///< global variable containing app state
EXTERN plexil_msg_t plexilMsg;

/**
 * Entry point for app
 */
void PLEXIL_AppMain(void);

/**
 * Function to initialize app properties
 */
void PLEXIL_AppInit(void);

/**
 * Function to clean up variables
 */
void PLEXIL_AppCleanUp(void);

/**
 * Function to process SB messages
 */
void PLEXIL_ProcessPacket();

/**
 * Function to run one interation of the PLEXIL executive loop
 */

void PLEXIL_Run();

/**
 * Function to validate plexil table parameters
 * @param TblPtr pointer to table
 * @return 0 for success.
 */

int32_t PlexilTableValidationFunc(void *TblPtr);

/**
 * Function to process commands from plexil
 * @param Msg PlexilCommandMsg
 * @return 1 if command is handled by default adapter,0 otherwise.
 */
uint8_t ProcessPlexilCommand(PlexilCommandMsg* Msg);

/**
 * Function to process lookups from plexil
 * @param Msg Plexil Command Msg
 * @return 1 if command is handled by default adapter,0 otherwise.
 */
uint8_t ProcessPlexilLookup(PlexilCommandMsg* Msg);

/**
 * Function to acquire flight data for plexil
 */
void PLEXIL_DAQ(void);


/**
 * Functions specific to default adapter implementing commands and lookups
 */
uint8_t runIdleChecks();
uint8_t ThrottleUp();

#endif //CFETOP_PLEXIL_H
