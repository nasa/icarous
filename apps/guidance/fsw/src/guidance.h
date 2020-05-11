/**
 * @file Guidance.h
 * @brief Function declaration, MACRO and data structure definitions for the Guidance application
 */

#ifndef ICAROUS_CFS_GUIDANCE_H
#define ICAROUS_CFS_GUIDANCE_H
#define GUIDANCE

#include "cfe.h"
#include "cfe_error.h"
#include "cfe_evs.h"
#include "cfe_sb.h"
#include "cfe_es.h"
#include "cfe_platform_cfg.h"

#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "Icarous_msg.h"
#include "Icarous_msgids.h"
#include "trajectory_msgids.h"
#include "guidance_msg.h"
#include "guidance_msgids.h"
#include "sch_msgids.h"
#include "UtilFunctions.h"

#include "Guidance.h"

#define GUIDANCE_STARTUP_INF_EID 0
#define GUIDANCE_COMMAND_ERR_EID 1

#define GUIDANCE_PIPE_DEPTH 30 
#define GUIDANCE_PIPE_NAME "GUIDANCE_PIPE"
#define GUIDANCE_MAJOR_VERSION 1
#define GUIDANCE_MINOR_VERSION 0

/**
 * @defgroup GUIDANCE
 * @brief Application to monitor traffic conflicts
 * @ingroup APPLICATIONS
 *
 * @details Description of this application
 * 
 */



/**
 * @struct GuidanceAppData_t
 * @brief Structure to hold app data
 */
typedef struct{
    CFE_SB_PipeId_t    guidance_Pipe;       ///< Pipe variable
    CFE_SB_MsgPtr_t    guidance_MsgPtr;     ///< Msg pointer to SB message
    GuidanceParams_t   guidance_params;     ///< Guidance table
    bool takeoffComplete;
    position_t pos;
    void* Guidance;
    char activePlan[25];
    status_t statustxt;
}guidanceAppData_t;

/**
 * App entry point
 */
void GUIDANCE_AppMain(void);

/**
 * Initializes/setup cfs app related entities.
 */
void GUIDANCE_AppInit(void);

/**
 * Initialize app specific data structures
 */
void GUIDANCE_AppInitData();

/**
 * Deallocates memory
 */
void GUIDANCE_AppCleanUp();

/**
 * Processes incoming software bus messages
 */
void GUIDANCE_ProcessPacket();

/**
 * Main function that computes guidance
 */
void GUIDANCE_Run();

/**
 * Validates table parameters
 * @param TblPtr Pointer to table
 * @return  0 if success
 */
int32_t GuidanceTableValidationFunc(void *TblPtr);

/**
 * Handle input commands for guidance app
 */
void HandleGuidanceCommands(argsCmd_t *cmd);

/**
 * Compute guidance commands for takeoff 
 */
void ComputeTakeoffGuidanceInput();


/**
 * @var Global variable to store all app related data
 */
EXTERN guidanceAppData_t guidanceAppData;


#endif //ICAROUS_CFS_GUIDANCE_H
