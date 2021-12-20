/**
 * @file cognition.h
 * @brief function declarations, definitions of macros, datastructures and global variables for the cognition application
 */
#ifndef _cognition_h_
#define _cognition_h_

#include "cfe.h"
#include "cfe_error.h"
#include "cfe_evs.h"
#include "cfe_sb.h"
#include "cfe_es.h"
#include "cfe_platform_cfg.h"

#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include "cognition_events.h"
#include "Icarous_msg.h"
#include "traffic_msg.h"
#include "trajectory_msg.h"
#include "geofence_msg.h"
#include "guidance_msg.h"
#include "Icarous_msgids.h"
#include "sch_msgids.h"
#include "traffic_msgids.h"
#include "trajectory_msgids.h"
#include "geofence_msgids.h"
#include "guidance_msgids.h"
#include "UtilFunctions.h"
#include "Guidance.h"

#include "Core/Cognition/Cognition.h"

#ifdef APPDEF_MERGER
#include "merger_msgids.h"
#include "merger_msg.h"
#endif

#ifdef APPDEF_SAFE2DITCH
#include "safe2ditch_msg.h"
#include "safe2ditch_msgids.h"
#endif

#define COGNITION_PIPE_NAME "COGPIPE"
#define COGNITION_PIPE_DEPTH 100

#define SCH_COGNITION_PIPE_NAME "SCH_COGNITION"

/**
 * @defgroup COGNITION_cFS_APP
 *
 * @details  Wrapper around the core Cognition C++ module. Passes data from the software bus into the Cognition module.
 * Extracts outputs from the Cognition module and puts in on the software bus for other apps to consume. 
 */


/**
 * @struct appdataCog_t
 * @brief Structure to hold app data
 */
typedef struct{
    CFE_SB_PipeId_t    CognitionPipe;      ///< pipe variable
    CFE_SB_PipeId_t    SchPipe;            ///< pipe variable
    CFE_SB_MsgPtr_t    CogMsgPtr;          ///< msg pointer to SB message
    CFE_SB_MsgPtr_t    SchMsgPtr;          ///< msg pointer to SB message
    CFE_TBL_Handle_t   CogtblHandle;       ///< table handle

    position_t position;                   ///< position message
    attitude_t attitude;                   ///< attitude message

    flightplan_t flightplan1;              ///< Mission flight plan
    flightplan_t flightplan2;              ///< Contigency flight plan
    flightplan_t *fp;

    bands_t trkBands;
    bands_t gsBands;
    bands_t altBands;
    bands_t vsBands;

    algorithm_e searchAlgType;

    status_t statustxt;

    cognition_params_t parameters;

    void* cog;                              ///< Cognition module object


}appdataCog_t;

/**
 * Entry point for app
 * @brief This is the main entry point for your application that will be used by cFS
 */
void COGNITION_AppMain(void);

/**
 * Initialize app properties
 * @brief Initialize app data: set up pipes, subscribe to messages, read table, initialize data
 */
void COGNITION_AppInit(void);

/**
 * Initialize app specific data
 */
void COGNITION_AppInitData(void);

/**
 * Clean up variables
 */
void COGNITION_AppCleanUp(void);

/**
 * Process data from the SB message
 * @brief Read data from software bus and send data to autopilot
 */
void COGNITION_ProcessSBData(void);

/**
 * Process ADSB data
 * @brief Read ADSB data from ADSB transponder and publish traffic data on software bus.
 */
void COGNITION_GetADSBData(void);

void COGNITION_DecisionProcess(void);

/**
 * Validate table data
 * @param *TblPtr pointer to table
 */
int32_t cognitionTableValidationFunc(void *TblPtr);


void COGNITION_SendGuidanceVelCmd(VelocityCommand cmd);

void COGNITION_SendGuidanceFlightPlan(FpChange cmd);

void COGNITION_SendGuidanceP2P(P2PCommand cmd);

void COGNITION_SendSpeedChange(SpeedChange cmd);

void COGNITION_SendAltChange(AltChange cmd);

void COGNITION_SendTakeoff(TakeoffCommand cmd);

void COGNITION_SendLand(LandCommand cmd);

void COGNITION_FindNewPath(FpRequest cmd);

void COGNITION_SendDitchRequest(DitchCommand cmd);

void COGNITION_SendRtlRequest(RtlCommand rtl_command);

EXTERN appdataCog_t appdataCog;
#endif /* _apInterface_h_ */
