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
#include "cognition_core.h"

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
 * @defgroup COGNITION
 * @brief Application for decision making
 * @ingroup APPLICATIONS
 *
 * @details  *
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

    flightplan_monitor_t fp1monitor;
    flightplan_monitor_t fp2monitor;

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

    cognition_t cog;

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
void COGNITION_GetADSBData();

void COGNITION_DecisionProcess();

/**
 * Validate table data
 * @param *TblPtr pointer to table
 */
int32_t cognitionTableValidationFunc(void *TblPtr);


void COGNITION_SendGuidanceVelCmd();

void COGNITION_SendGuidanceFlightPlan();

void COGNITION_SendGuidanceP2P();

void COGNITION_SendSpeedChange();

void COGNITION_SendTakeoff();

void COGNITION_SendLand();

void COGNITION_FindNewPath();

void COGNITION_SendDitchRequest();

EXTERN appdataCog_t appdataCog;
#endif /* _apInterface_h_ */
