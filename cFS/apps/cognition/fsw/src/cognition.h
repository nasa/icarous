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

#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "cognition_events.h"
#include "msgdef/ardupilot_msg.h"
#include "msgdef/traffic_msg.h"
#include "msgdef/trajectory_msg.h"
#include "msgids/ardupilot_msgids.h"
#include "msgids/scheduler_msgids.h"
#include "msgids/traffic_msgids.h"

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
 * @enum flightphase_e
 * @brief Enumerations of the various flight phases
 */
typedef enum{
    IDLE,
    TAXI,
    TAKEOFF,
    CLIMB,
    CRUISE,
    DESCENT,
    APPROACH,
    LANDING
}flightphase_e;

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
    object_t traffic;                      ///< traffic message
    int currentWP;                         ///< autopilot's current waypoint
    flightplan_t flightplan;

    flightphase_e fpPhase;
    int missionStart;
    int armStatus;
    int takeoffStatus;
    bool takeoffStarted;
    double takeoffAlt;
    int nextMissionWP;
    int totalMissionWP;
    bool keepInConflict;
    bool keepOutConflict;
    double recoveryPosition[3];
    bool trafficConflict;
    bool trafficTrackConflict;
    bool trafficSpeedConflict;
    bool trafficAltConflict;
    bool flightPlanConflict;
    double allowedXtrackError;
    double xtrackError;
    double resolutionSpeed;
    algorithm_e searchType;
    int totalFences;
    int totalTraffic;
    int nextFeasibleWP;
    bool directPathToFeasibleWP;
    flightplan_t resolutionTraj;
    flightplan_t missionFP;
    double interceptManeuver[3];
    double interceptHeadingToPlan;
    int trafficResType;
    double preferredTrack;
    double preferredSpeed;
    double preferredAlt;
    bands_t trkBands;
    bands_t gsBands;
    bands_t altBands;

    double ditchsite[3];
    bool ditch;
    bool resetDitch;
    bool endDitch;
    bool ditchGuidanceRequired;
    bool ditchRequested;


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
 * Clean up variables
 */
void COGNITION_AppCleanUp(void);

/**
 * Initialize ports to the autopilot
 * @brief Open ports to autopilot and intialize ports here
 */
void COGNITION_InitializeAPPorts();

/**
 * Process data from the autopilot
 * @brief Read data from the autopilot and construct software  bus messages
 */
void COGNITION_ProcessAPData(void);

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

/**
 * Validate table data
 * @param *TblPtr pointer to table
 */
int32_t cognitionTableValidationFunc(void *TblPtr);

/**
 * Top level finite state machines governing flight phase transitions
 */
void FlightPhases(void);

/**
 * Function to handle conflict management
 */
void ConflictManagement(void);

EXTERN appdataCog_t appdataCog;

#endif /* _apInterface_h_ */