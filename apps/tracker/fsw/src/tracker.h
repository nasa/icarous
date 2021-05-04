/**
 * @file tracker.h
 * @brief Function declaration, MACRO and data structure definitions for the tracker application
 */

#ifndef ICAROUS_CFS_TRACKER_H
#define ICAROUS_CFS_TRACKER_H

#include "cfe.h"
#include "cfe_error.h"
#include "cfe_evs.h"
#include "cfe_sb.h"
#include "cfe_es.h"
#include <string.h>

#include "sch_msgids.h"
#include "tracker_tbl.h"
#include <time.h>

#define TRACKER_STARTUP_INF_EID 0
#define TRACKER_COMMAND_ERR_EID 1

#define TRACKER_PIPE_DEPTH 30 
#define TRACKER_PIPE_NAME "TRACKER_PIPE"
#define TRACKER_MAJOR_VERSION 1
#define TRACKER_MINOR_VERSION 0

/**
 * @defgroup TRACKER
 * @brief Application to monitor traffic conflicts
 * @ingroup APPLICATIONS
 *
 * @details Description of this application
 * 
 */


/**
 * @struct trackerAppData_t
 * @brief Structure to hold app data
 */
typedef struct{
    CFE_SB_PipeId_t    tracker_Pipe;       ///< Pipe variable
    CFE_SB_MsgPtr_t    tracker_MsgPtr;     ///< Msg pointer to SB message
    CFE_TBL_Handle_t   tracker_tblHandle;    ///< Traffic table handle
    bool initialized;
    void* TargetTracker;
}trackerAppData_t;

/**
 * App entry point
 */
void TRACKER_AppMain(void);

/**
 * Initializes data structures and cfs related entities.
 */
void TRACKER_AppInit(void);

/**
 * Deallocates memory
 */
void TRACKER_AppCleanUp(void);

/**
 * Processes incoming software bus messages
 */
void TRACKER_ProcessPacket(void);

/**
 * Validates table parameters
 * @param TblPtr Pointer to table
 * @return  0 if success
 */
int32_t trackerTableValidationFunc(void *TblPtr);

/**
 * @var Global variable to store all app related data
 */
trackerAppData_t trackerAppData;

#endif //ICAROUS_CFS_TRACKER_H
