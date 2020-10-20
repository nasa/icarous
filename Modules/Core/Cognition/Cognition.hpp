#ifndef COGNITION_CORE_H
#define COGNITION_CORE_H

//#define _GNU_SOURCE

#include <string>
#include <cstring>
#include <list>
#include <map>
#include <vector>
#include <fstream>
#include <cstdio>
#include <cmath>
#include <memory>


#include "Interfaces.h"
#include "Position.h"
#include "Velocity.h"
#include "Projection.h"
#include "EuclideanProjection.h"
#include "Units.h"
#include "Plan.h"

#include "Commands.hpp"

#include "EventManager.hpp"
#include "EventHandler.hpp"

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
    DITCH_RESOLUTION,
    TRACK_SPEED_VS_RESOLUTION
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
    double utcTime;                ///< Current time
    std::string timeString;        ///< Formatted time string
    std::string callSign;          ///< Vehicle's callsign string
    cognition_params_t parameters; ///< Configurable parameters used by Cognition

    // Flight plan book keeping
    std::list<larcfm::Plan> flightPlans; ///< List of flight plans
    larcfm::Plan *activePlan;            ///< Pointer to the active flight plan
    std::map<std::string, int> nextWpId; ///< Map from flight plan id to next waypoint id

    int nextFeasibleWpId;      ///< to next feasible waypoint id
    bool closestPointFeasible; ///< Feasibility of nearest point on primary flight plan
    resolutionType_e resType;

    bool primaryFPReceived;
    bool recovery[4];
    bool validResolution[4];
    double scenarioTime;
    double timeToTrafficViolation1; ///< time in to violation from DAA
    double timeToTrafficViolation2; ///< time out of violation from DAA
    double timeToTrafficViolation3; ///< time in to violation from Trajectory Monitor
    double timeToFenceViolation;

    double xtrackDeviation;

    // Aircraft state information
    larcfm::Position position;
    larcfm::Velocity velocity;
    double hdg;
    double speed;
    double refWpTime;
    bool wpMetricTime;

    // Geofence conflict related variables
    bool keepInConflict;
    bool keepOutConflict;
    larcfm::Position recoveryPosition;
    larcfm::Position clstPoint;

    // Traffic conflict related variables
    bool trafficConflict;
    bool allTrafficConflicts[4];
    bool newAltConflict;
    double trafficConflictStartTime;

    // Conflict flags based on flightplan projections
    bool planProjectedTrafficConflict;
    bool planProjectedFenceConflict;

    bool returnSafe;
    double resolutionStartSpeed;

    double preferredTrack;
    double preferredSpeed;
    double preferredAlt;
    double preferredVSpeed;

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

    int gsBandNum;
    int gsBandType[20];
    double gsBandMin[20];
    double gsBandMax[20];

    int altBandNum;
    int altBandType[20];
    double altBandMin[20];
    double altBandMax[20];

    // Flight plan monitoring related variables
    bool XtrackConflict;
    bool directPathToFeasibleWP1;
    bool directPathToFeasibleWP2;

    // Ditching variables
    larcfm::Position ditchSite;
    double todAltitude;
    bool ditch;
    bool resetDitch;
    bool endDitch;
    bool ditchRouteFeasible;

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
    int requestGuidance2NextWP; // -1: undediced, 0: no guidance, 1: obtain guidance
    bool p2pComplete;
    bool topOfDescent;

    // Status of merging activity
    uint8_t mergingActive;
    uint16_t numSecPaths;

    // output
    std::list<Command> cognitionCommands;

    std::ofstream log;
}CognitionState_t;

class Cognition{
    public:
        Cognition(const std::string callSign);

        void Reset();

        void ResetFlightPhases();

        void InputVehicleState(const larcfm::Position &pos,const larcfm::Velocity &vel,const double heading);

        void InputFlightPlanData(const std::string &plan_id,const std::list<waypoint_t> &waypoints,const double initHeading,bool repair);

        void InputTrajectoryMonitorData(const trajectoryMonitorData_t& tjMonData);

        void InputParameters(const cognition_params_t &new_params);

        void InputDitchStatus(const larcfm::Position &ditch_site,const double todAltitude,const bool ditch_requested);

        void InputMergeStatus(const int merge_status);

        void InputTrackBands(const bands_t &track_bands);

        void InputSpeedBands(const bands_t &speed_bands);

        void InputAltBands(const bands_t &alt_bands);

        void InputVSBands(const bands_t &vs_bands);

        void InputGeofenceConflictData(const geofenceConflict_t &gf_conflict);

        void ReachedWaypoint(const std::string &plan_id, const int wp_reached_id);

        int GetCognitionOutput(Command &command);

        void StartMission(const int mission_start_value,const double scenario_time);

        void Run(double time);

        CognitionState_t cogState;

    private:
        EventManagement<CognitionState_t> eventMng;
        void InitializeState();
        void InitializeEvents();
        void InitializeEventHandlers();

};

larcfm::Plan* GetPlan(std::list<larcfm::Plan>* planList,const std::string &plan_id);

int GetTotalWaypoints(std::list<larcfm::Plan>* planList,const std::string &plan_id);

larcfm::Position GetNextWP(larcfm::Plan*fp, int nextWP);

larcfm::Position GetPrevWP(larcfm::Plan*fp, int nextWP);

larcfm::Velocity GetNextWPVelocity(larcfm::Plan*fp, int nextWP);

bool CheckProjectedTrafficConflict(larcfm::Position position, larcfm::Velocity velocity,
                                   larcfm::Position prevWP,larcfm::Position nextWP,
                                   double timeToViolation,double DTHR,double ZTHR);

bool ComputeTargetFeasibility(CognitionState_t *state,larcfm::Position target);

void SetGuidanceVelCmd(CognitionState_t *state,const double track,const double gs,const double vs);

void SetGuidanceSpeedCmd(CognitionState_t *state,const std::string &planID,const double speed,const int hold = 0);

void SetGuidanceAltCmd(CognitionState_t *state,const std::string &planID,const double alt,const int hold = 0);
        
void SetGuidanceFlightPlan(CognitionState_t *state,const std::string &plan_id,const int wp_index);

void SetLandCmd(CognitionState_t* state);

void SetDitchSiteRequestCmd(CognitionState_t* state);

void FindNewPath(CognitionState_t *state,const std::string &planID,const larcfm::Position &positionA,
                const larcfm::Velocity &velocityA,
                const larcfm::Position &positionB,
                const larcfm::Velocity &velocityB);

void SetGuidanceP2P(CognitionState_t *state,const larcfm::Position &point,const double speed);

void SendStatus(CognitionState_t *state,const char buffer[],const uint8_t severity);

resolutionType_e GetResolutionType(CognitionState_t* state);

void LogMessage(CognitionState_t* state,std::string message);

// Cognition util functions
double ComputeHeading(const larcfm::Position &positionA,const larcfm::Position &positionB);
double ComputeXtrackDistance(const larcfm::Position &prev_wp,
                            const larcfm::Position &next_wp,
                            const larcfm::Position &pos,
                            double offset[]);
larcfm::Position GetNearestPositionOnPlan(const larcfm::Position &prev_wp,
                        const larcfm::Position &next_wp,
                        const larcfm::Position &current_pos);
void ManeuverToIntercept(const larcfm::Position &prev_wp,
                            const larcfm::Position &next_wp,
                            const larcfm::Position &curr_position,
                            double x_trk_dev_gain,
                            const double resolution_speed,
                            const double allowed_dev,
                            larcfm::Velocity &output_velocity);
double GetInterceptHeadingToPlan(const larcfm::Position &prev_wp,
                                    const larcfm::Position &next_wp,
                                    const larcfm::Position &current_pos);
bool CheckTurnConflict(const double low,
                    const double high,
                    const double new_heading,
                    const double old_heading);



#endif
