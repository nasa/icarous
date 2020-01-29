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
#include "cognition_events.h"
#include "Icarous_msg.h"
#include "traffic_msg.h"
#include "trajectory_msg.h"
#include "geofence_msg.h"
#include "guidance_msg.h"
#include "merger_msg.h"
#include "Icarous_msgids.h"
#include "sch_msgids.h"
#include "traffic_msgids.h"
#include "trajectory_msgids.h"
#include "geofence_msgids.h"
#include "guidance_msgids.h"
#include "merger_msgids.h"

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
    IDLE_PHASE,
    TAXI_PHASE,
    TAKEOFF_PHASE,
    CLIMB_PHASE,
    CRUISE_PHASE,
    DESCENT_PHASE,
    APPROACH_PHASE,
    LANDING_PHASE,
    MERGING_PHASE
}flightphase_e;

/**
 * @enum status_e
 * @brief output status of each flight phase state
 */
typedef enum{
    SUCCESS,
    FAILED,
    RUNNING,
    INITIALIZING
}status_e;

/**
 * @enum resolutionType_e
 * @brief Enumeration of resolution types
 */
typedef enum{
    SPEED_RESOLUTION,
    ALTITUDE_RESOLUTION,
    TRACK_RESOLUTION,
    VERTICALSPEED_RESOLUTION,
    SEARCH_RESOLUTION
}resolutionType_e;

/**
 * @enum conflictStatus_e
 * @brief conflict status
 */
typedef enum{
    NOOPC,
    INITIALIZE,
    COMPUTE,
    RESOLVE,
    COMPLETE
}conflictState_e;


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

    flightphase_e fpPhase;                 ///< Current phase of flight

    position_t position;                   ///< position message
    attitude_t attitude;                   ///< attitude message
    double speed;                          ///< Current speed of the vehicle

    flightplan_t flightplan1;              ///< Mission flight plan
    flightplan_t flightplan2;              ///< Contigency flight plan
    flightplan_t *fp;

    int missionStart;

    int nextPrimaryWP;
    int nextSecondaryWP;
    int nextFeasibleWP1;
    int nextFeasibleWP2;
    int nextWP;
    char currentPlanID[10];
    bool Plan0;
    bool Plan1;
    double resolutionSpeed;

    flightplan_monitor_t fp1monitor;
    flightplan_monitor_t fp2monitor;

    // Geofence conflict related variables
    bool keepInConflict;
    bool keepOutConflict;
    double recoveryPosition[3];

    // Traffic conflict related variables
    bool trafficConflict;
    bool trafficTrackConflict;
    bool trafficSpeedConflict;
    bool trafficAltConflict;

    // Flight plan related variables
    bool XtrackConflict1;
    bool XtrackConflict2;
    bool directPathToFeasibleWP1;
    bool directPathToFeasibleWP2;

    int trafficResType;
    double preferredTrack;
    double preferredSpeed;
    double preferredAlt;
    bands_t trkBands;
    bands_t gsBands;
    bands_t altBands;
    bands_t vsBands;

    double DTHR;
    double ZTHR;

    double ditchsite[3];
    bool ditch;
    bool resetDitch;
    bool endDitch;
    bool ditchGuidanceRequired;
    bool ditchRequested;
    bool returnSafe;

    resolutionType_e resolutionTypeCmd;
    double prevResSpeed;
    double prevResAlt;
    double prevResTrack;
    double prevResVspeed;

    status_e takeoffState;
    status_e cruiseState;
    int takeoffComplete;


    conflictState_e geofenceConflictState;
    conflictState_e XtrackConflictState;
    conflictState_e trafficConflictState;
    conflictState_e return2NextWPState;


    int8_t request;
    algorithm_e searchAlgType;

    int requestGuidance2NextWP;             // -1: undediced, 0: no guidance, 1: obtain guidance
    bool p2pcomplete;
    bool fp1complete;
    bool fp2complete;

    // Status of merging activity
    bool mergingActive;     

    status_t statustxt;
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

/**
 * Top level finite state machines governing flight phase transitions
 */
void FlightPhases(void);

/**
 * Function to handle conflict management
 */
bool TrafficConflictManagement(void);

void CheckDirectPathFeasibility(position_t posA,waypoint_t posB);

void FindNewPath(algorithm_e searchType, double positionA[],double velocityA[],double positionB[]);

bool GeofenceConflictManagement(void);

bool ReturnToNextWP(void);

bool XtrackManagement(void);

void GetResolutionType(void);

status_e Takeoff(void);

status_e Climb(void);

status_e Cruise(void);

status_e Descent(void);

status_e Approach(void);

status_e Landing(void);

bool RunTrafficResolution(void);

void SetGuidanceVelCmd(double track,double gs,double vs);

void SetGuidanceFlightPlan(char name[],int nextWP);

void SetGuidanceP2P(double lat,double lon,double alt,double speed);

void ResetFlightPhases(void);

EXTERN appdataCog_t appdataCog;

#endif /* _apInterface_h_ */
