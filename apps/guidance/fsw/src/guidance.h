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

#include "guidance_tbl.h"
#include "Icarous_msg.h"
#include "Icarous_msgids.h"
#include "trajectory_msgids.h"
#include "guidance_msg.h"
#include "guidance_msgids.h"
#include "sch_msgids.h"
#include "UtilFunctions.h"

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
    guidanceTable_t    guidance_tbl;        ///< Guidance table
    flightplan_t primaryFlightPlan;         ///< Primary mission flight plan
    flightplan_t secondaryFlightPlan;       ///< Detour flight plan (for rerouting/contingencies)
    argsCmd_t command;                      ///< Guidance mode commands from external source
    guidance_mode_e guidanceMode;           ///< Guidance mode
    position_t position;                    ///< ownship position
    int nextPrimaryWP;                      ///< index of next mission WP
    int nextSecondaryWP;                    ///< index of next secondary flightplan WP
    argsCmd_t velCmd;                       ///< output velocity commands
    bool takeoffComplete;                   ///< Status of takeoff phase
    double point[3];                        ///< Position command
    double pointSpeed;                      ///< Speed to follow for point2point commands
    bool reachedStatusUpdated;              ///< Flag to indicate if mission item has been reached
    double refSpeed;                        ///< Reference speed
    double capRScaling;                     ///< Capture radius scaling
    status_t statustxt;                     ///< Status txt messages
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
 * Compute guidance input for a given flight plan
 */
int ComputeFlightplanGuidanceInput(flightplan_t* fp, int nextWP);

/**
 * Computes the point on the flight plan to fly towards
 */
bool ComputeOffSetPositionOnPlan(flightplan_t *fp,double position[],int currentLeg,double outputLLA[]);

/**
 * Compute the correct intersection with the circle and flight plan
 */
void GetCorrectIntersectionPoint(double _wpA[],double _wpB[],double heading,double r,double output[]);


/**
 * Point to point control
 */
bool Point2PointControl();


/**
 * Publish guidance status
 */
void PublishGuidanceStatus();


/**
 * @var Global variable to store all app related data
 */
EXTERN guidanceAppData_t guidanceAppData;


#endif //ICAROUS_CFS_GUIDANCE_H
