/**
 * @file merger.h
 * @brief Definition of defines, structs and functions for merger app
 *
 * @author Swee Balachandran
 *
 */
#ifndef ICAROUS_CFS_MERGER_H
#define ICAROUS_CFS_MERGER_H

#include "cfe.h"
#include "cfe_error.h"
#include "cfe_evs.h"
#include "cfe_sb.h"
#include "cfe_es.h"
#include "cfe_platform_cfg.h"
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <float.h>


#include "Icarous.h"
#include "Merger.h"
#include "merger_msg.h"
#include "merger_msgids.h"
#include "merger_table.h"
#include "raft_msgids.h"
#include "raft_msg.h"
#include "sch_msgids.h"

#include "Guidance.h"
#include "guidance_msg.h"
#include "guidance_msgids.h"

#define MERGER_STARTUP_INF_EID 0
#define MERGER_COMMAND_ERR_EID 1
#define MERGER_RECEIVED_FENCE_EID 3

#define MERGER_PIPE_DEPTH 100
#define MERGER_PIPE_NAME "MERGER_PIPE"
#define MERGER_MAJOR_VERSION 1
#define MERGER_MINOR_VERSION 0


/**
 * @defgroup MERGER_cFS_APP 
 *
 * @details Wrapper for the merger C++ module
 */




typedef flightplan_t mergePoints_t;

/**
 * @struct mergerAppData_t
 * @brief  Struct to hold merger app data
 */
typedef struct{
    CFE_SB_PipeId_t    Merger_Pipe;       ///< Pipe variable
    CFE_SB_MsgPtr_t    Merger_MsgPtr;     ///< Pointer to SB message
    CFE_TBL_Handle_t   Merger_tblHandle;  ///< Table handle
    mergePoints_t mgData;                 ///< Merge points
    void* merger;
    MergerTable_t mergerTable;            ///< merger table data
    mergeStatus_e mgStatus;
}mergerAppData_t;

/**
 * App's main entry point
 */
void MERGER_AppMain(void);

/**
 * App initialization
 */
void MERGER_AppInit(void);

/**
 * Initialize app data
 */
void MERGER_AppInitializeData(void);

/**
 * App clean up
 */
void MERGER_AppCleanUp(void);

/**
 * Function to process received packets
 */
void MERGER_ProcessPacket(void);


/**
 * Function to validate table parameters
 * @param TblPtr pointer to table
 * @return 0 if success
 */
int32_t MergerTableValidationFunc(void *TblPtr);

void MERGER_WriteLogEntry(void);

#endif //ICAROUS_CFS_MERGER_H
