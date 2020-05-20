#ifndef COGNITION_CORE_H
#define COGNITION_CORE_H

#define _GNU_SOURCE

#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "UtilFunctions.h"


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
    EMERGENCY_DESCENT_PHASE,
    APPROACH_PHASE,
    LANDING_PHASE,
    MERGING_PHASE
}flightphase_e;

typedef enum{
    cPRIMARY_FLIGHTPLAN,     ///< Follow the primary mission flight plan
    cSECONDARY_FLIGHTPLAN,   ///< Follow the secondary flight plan (for detours/reroutes)
    cVECTOR,                 ///< Follow the given velocity vector
    cPOINT2POINT,            ///< Fly to a specific position
    cORBIT,                  ///< Orbit around a given point
    cHELIX,                  ///< A helical orbit
    cTAKEOFF,                ///< Takeoff mode
    cLAND,                   ///< Landing mode
    cSPEED_CHANGE,           ///< Speed change command
    cNOOP                    ///< No operations
}command_mode_e;

/**
 * @enum status_e
 * @brief output status of each flight phase state
 */
typedef enum{
    NOOPS,
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
    SEARCH_RESOLUTION,
    DITCH_RESOLUTION
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

typedef enum{
    REQUEST_NIL,
    REQUEST_PROCESSING,
    REQUEST_RESPONDED
}request_e;

typedef struct{

    uint64_t UTCtime;                    ///< Current time

    // Flight plan book keeping
    int nextPrimaryWP;
    int nextSecondaryWP;
    int nextFeasibleWP1;
    int nextFeasibleWP2;
    int nextWP;

    char currentPlanID[10];
    bool Plan0;
    bool Plan1;
    bool primaryFPReceived;
    double scenarioTime;
    int num_waypoints;
    int num_waypoints2;


    double wpPrev1[3];
    double wpPrev2[3];
    double wpNext1[3];
    double wpNext2[3];
    double wpNextFb1[3];
    double wpNextFb2[3];

    // Aircraft state information
    double position[3];
    double velocity[3];
    double hdg;
    double speed;
    double resolutionSpeed;
    double refWPTime;
    bool wpMetricTime;

    // Geofence conflict related variables
    bool keepInConflict;
    bool keepOutConflict;
    double recoveryPosition[3];

    // Traffic conflict related variables
    bool trafficConflict;
    bool trafficTrackConflict;
    bool trafficSpeedConflict;
    bool trafficAltConflict;

    bool returnSafe;

    int trafficResType;
    double preferredTrack;
    double preferredSpeed;
    double preferredAlt;
    double DTHR;
    double ZTHR;

    int vsBandsNum;
    double resVUp;
    double resVDown;

    double prevResSpeed;
    double prevResAlt;
    double prevResTrack;
    double prevResVspeed;

    int trkBandNum;
    int trkBandType[20];
	double trkBandMin[20];
	double trkBandMax[20];

    resolutionType_e resolutionTypeCmd;

    // Flight plan monitoring related variables
    bool XtrackConflict1;
    bool XtrackConflict2;
    bool directPathToFeasibleWP1;
    bool directPathToFeasibleWP2;

    double xtrackDeviation1;
    double xtrackDeviation2;
    double allowedXtrackDev1;
    double allowedXtrackDev2;

    bool nextWPFeasibility1;
    bool nextWPFeasibility2;

    double xtrkGain;  //TODO

    bool fp1ClosestPointFeasible;
    bool fp2ClosestPointFeasible;

    // Ditching variables
    double ditchsite[3];
    bool ditch;
    bool resetDitch;
    bool endDitch;
    bool ditchRouteFeasible;

    // State machine related variables
    flightphase_e fpPhase;                 ///< Current phase of flight

    status_e takeoffState;
    status_e cruiseState;
    status_e emergencyDescentState;
    int takeoffComplete;

    conflictState_e geofenceConflictState;
    conflictState_e XtrackConflictState;
    conflictState_e trafficConflictState;
    conflictState_e return2NextWPState;

    int8_t request;

    int missionStart;
    int requestGuidance2NextWP;             // -1: undediced, 0: no guidance, 1: obtain guidance
    bool p2pcomplete;
    bool fp1complete;
    bool fp2complete;
    bool topofdescent;

    // Status of merging activity
    uint8_t mergingActive;

    // output
    uint8_t guidanceCommand;
    double cmdparams[10];
    char statusBuf[250];
    uint8_t statusSeverity;
    bool sendCommand;
    bool sendStatusTxt;
    bool sendStatusWPReached;
    bool pathRequest;
    uint8_t searchType;
    double startPosition[3];
    double stopPosition[3];
    double startVelocity[3];
}cognition_t;


void InitializeCognition(cognition_t* cog);

/**
 * Top level finite state machines governing flight phase transitions
 */
void FlightPhases(cognition_t* cog);

/**
 * Function to handle conflict management
 */
bool TrafficConflictManagement(cognition_t* cog);

void FindNewPath(cognition_t *cog,uint8_t searchType, double positionA[],double velocityA[],double positionB[]);

bool GeofenceConflictManagement(cognition_t *cog);

bool ReturnToNextWP(cognition_t *cog);

bool XtrackManagement(cognition_t *cog);

bool TimeManagement(cognition_t *cog);

void GetResolutionType(cognition_t *cog);

status_e Takeoff(cognition_t *cog);

status_e Climb(cognition_t *cog);

status_e Cruise(cognition_t *cog);

status_e Descent(cognition_t *cog);

status_e Approach(cognition_t *cog);

status_e Landing(cognition_t *cog);

status_e EmergencyDescent(cognition_t *cog);

bool RunTrafficResolution(cognition_t *cog);

void SetGuidanceVelCmd(cognition_t *cog,double track,double gs,double vs);

void SetGuidanceSpeedCmd(cognition_t* cog, double speed);

void SetGuidanceFlightPlan(cognition_t *cog,char name[],int nextWP);

void SetGuidanceP2P(cognition_t *cog,double lat,double lon,double alt,double speed);

void ResetFlightPhases(cognition_t *cog);

void SendStatus(cognition_t *cog,char buffer[],uint8_t severity);

#endif
