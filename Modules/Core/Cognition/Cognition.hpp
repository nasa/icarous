/**
 * @file Cognition.hpp
 * @brief Cognition module definitions 
 */

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
#include <set>


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
 * @enum command_mode_e 
 * @brief command types
 */
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
 * @enum request_e 
 * @brief Enuemration of flightplan request states
 */
typedef enum{
    REQUEST_NIL,
    REQUEST_PROCESSING,
    REQUEST_RESPONDED
}request_e;

/**
 * @enum takeoff_status_e 
 * @brief Enumeration of takeoff status
 */
typedef enum{
    TAKEOFF_COMPLETE,
    TAKEOFF_INPROGESS,
    TAKEOFF_INACTIVE
}takeoff_status_e;

/**
 * @brief state containing all data required for decision making
 * 
 */
typedef struct{
    double utcTime;                      ///< Current time
    std::string timeString;              ///< Formatted time string
    std::string callSign;                ///< Vehicle's callsign string
    std::string missionPlan;             ///< Mission plan id (should be "Plan0")
    larcfm::Position launchPoint;        ///< Position for RTL
    cognition_params_t parameters;       ///< Configurable parameters used by Cognition
    bool icReady;                        ///< True if icarous is ready

    // Flight plan book keeping
    std::list<larcfm::Plan> flightPlans; ///< List of flight plans
    larcfm::Plan *activePlan;            ///< Pointer to the active flight plan
    std::map<std::string, int> nextWpId; ///< Map from flight plan id to next waypoint id

    int nextFeasibleWpId;                ///< Next feasible waypoint id
    bool closestPointFeasible;           ///< Feasibility of nearest point on primary flight plan
    resolutionType_e resType;            ///< Resolution type

    bool primaryFPReceived;              ///< Received primary flightplan
    bool recovery[4];                    ///< true if recovery is active in the 4 dimensions (speed,track,altitude,vs)
    bool validResolution[4];             ///< true if there is a valid resolution
    double scenarioTime;                 ///< flightplan start time
    double timeToTrafficViolation1;      ///< time in to violation from DAA
    double timeToTrafficViolation2;      ///< time out of violation from DAA
    double timeToTrafficViolation3;      ///< time in to violation from Trajectory Monitor
    double timeToFenceViolation;         ///< time to violate fence

    double xtrackDeviation;              ///< Cross track deviation

    larcfm::Position position;           ///< position
    larcfm::Velocity velocity;           ///< velocity
    double hdg;                          ///< heading
    double speed;                        ///< speed

    bool keepInConflict;                 ///< keep in geofence conflict
    bool keepOutConflict;                ///< keep out geofence conflict
    larcfm::Position recoveryPosition;   ///< recovery position (safe position)
    larcfm::Position clstPoint;          ///< closest point on nominal plan
    bool lineOfSight2Goal;               ///< true if line of sight to next feasible waypoint is available
    bool lineOfSight2GoalPrev;           ///< previous value of lineOfSight2Goal

    bool trafficConflict;                ///< true if there a traffic conflict
    bool allTrafficConflicts[4];         ///< traffic conflict with respect to each dimension
    bool newAltConflict;                 ///< new altitude conflict
    double trafficConflictStartTime;     ///< traffic conflict start time

    bool planProjectedTrafficConflict;   ///< traffic conflict based on projection of time to violation on current plan
    bool planProjectedFenceConflict;     ///< fence conflict based on projection of time to violation on current plan

    bool returnSafe;                     ///< safe to return to mission
    double resolutionStartSpeed;         ///< speed to start resolution

    double preferredTrack;               ///< track resolution
    double preferredSpeed;               ///< speed resolution
    double preferredAlt;                 ///< altitude resolution
    double preferredVSpeed;              ///< vertical speed resolution

    int vsBandsNum;                      ///< number of vertical speed bands
    double resVUp;                       ///< up vertical resolution
    double resVDown;                     ///< down vertical resolution

    std::set<std::string> conflictTraffics; ///< conflicting traffic ids

    double prevResSpeed;                 ///< previous speed resolution
    double prevResAlt;                   ///< previous altitude resolution
    double prevResTrack;                 ///< previous track resolution
    double prevResVspeed;                ///< previous vertical speed resolution

    int trkBandNum;                      ///< number of track bands
    int trkBandType[20];                 ///< band types
    double trkBandMin[20];               ///< min track bands
    double trkBandMax[20];               ///< max track bands

    int gsBandNum;                       ///< number of ground speed bands
    int gsBandType[20];                  ///< ground speed band type
    double gsBandMin[20];                ///< min ground speed 
    double gsBandMax[20];                ///< max ground speed

    int altBandNum;                      ///< number of altitude bands
    int altBandType[20];                 ///< altitude band types
    double altBandMin[20];               ///< min alt band
    double altBandMax[20];               ///< max alt band

    bool XtrackConflict;                 ///< cross track conflict
    bool directPathToFeasibleWP1;        ///< direct path to next feasible wp on primary plan
    bool directPathToFeasibleWP2;        ///< direct path to next feasible wp on secondary plan

    // Ditching variables
    larcfm::Position ditchSite;          ///< position of ditch site
    double todAltitude;                  ///< top of descent altitude
    bool ditch;                          ///< start ditching when true
    bool resetDitch;                     ///< reset ditching
    bool endDitch;                       ///< end ditching
    bool ditchRouteFeasible;             ///< true if direct path to ditch site is feasible

    bool leftTurnConflict;               ///< true if left turn is not possible
    bool rightTurnConflict;              ///< true if right turn is not possible

    takeoff_status_e takeoffState;       ///< takeoff state

    request_e pathRequest;               ///< status of path request

    int missionStart;                    ///< mission start waypoint
    int requestGuidance2NextWP;          ///< -1: undediced, 0: no guidance, 1: obtain guidance
    bool p2pComplete;                    ///< status of point 2 point guidance
    bool topOfDescent;                   ///< reached top of descent

    uint8_t mergingActive;               ///< status of merging activity
    uint16_t numSecPaths;                ///< number of secondary paths

    std::list<Command> cognitionCommands;///< output commands

    std::ofstream log;                   ///< log file handle
}CognitionState_t;

class Cognition{
    public:
        /**
         * @brief Construct a new Cognition object
         * 
         * @param callSign name for vehicle
         * @param config name of configuration file
         */
        Cognition(const std::string callSign,const std::string config);

        /**
         * @brief Read parameters from provided config file
         * 
         * @param config name of configuration file
         */
        void ReadParamsFromFile(const std::string config);

        /**
         * @brief Reset event management framework
         */
        void Reset();

        /**
         * @brief Input current aircraft state data
         * 
         * @param pos position information
         * @param vel velocity information
         * @param heading heading information
         */
        void InputVehicleState(const larcfm::Position &pos,const larcfm::Velocity &vel,const double heading);

        /**
         * @brief Input flightplan data 
         * 
         * @param plan_id name for plan
         * @param waypoints list of waypoints
         * @param initHeading initial Heading
         * @param repair true if plan should be converted into kinematic plan
         * @param repairTurnRate turn rate used to convert to kinematic plan
         */
        void InputFlightPlanData(const std::string &plan_id,const std::list<waypoint_t> &waypoints,
                                 const double initHeading,bool repair,double repairTurnRate);

        /**
         * @brief Input trajectory monitoring data 
         * 
         * @param tjMonData trajectory monitoring info
         */
        void InputTrajectoryMonitorData(const trajectoryMonitorData_t& tjMonData);

        /**
         * @brief  Input paramaters directly
         * 
         * @param new_params parameters
         */
        void InputParameters(const cognition_params_t &new_params);

        /**
         * @brief Input ditch status 
         * 
         * @param ditch_site position of ditch site
         * @param todAltitude top of descent altitude
         * @param ditch_requested initiate ditching
         */
        void InputDitchStatus(const larcfm::Position &ditch_site,const double todAltitude,const bool ditch_requested);

        /**
         * @brief Input merging status 
         * 
         * @param merge_status 
         */
        void InputMergeStatus(const int merge_status);

        /**
         * @brief Input track bands 
         * 
         * @param track_bands 
         */
        void InputTrackBands(const bands_t &track_bands);

        /**
         * @brief Input speed bands 
         * 
         * @param speed_bands 
         */
        void InputSpeedBands(const bands_t &speed_bands);

        /**
         * @brief Input altitude bands
         * 
         * @param alt_bands 
         */
        void InputAltBands(const bands_t &alt_bands);

        /**
         * @brief Input vertical speed bands
         * 
         * @param vs_bands 
         */
        void InputVSBands(const bands_t &vs_bands);

        /**
         * @brief Input geofence conflict data 
         * 
         * @param gf_conflict 
         */
        void InputGeofenceConflictData(const geofenceConflict_t &gf_conflict);

        /**
         * @brief Input waypoint reached data
         * 
         * @param plan_id  name of flightplan
         * @param wp_reached_id index of waypoint reached
         */
        void ReachedWaypoint(const std::string &plan_id, const int wp_reached_id);

        /**
         * @brief Get the Cognition Output object
         * 
         * @param command output command 
         * @return int remaining commands
         */
        int GetCognitionOutput(Command &command);

        /**
         * @brief start mission 
         * 
         * @param mission_start_value index of waypoint to which vehicle should start flight. 
         * @param scenario_time 
         */
        void StartMission(const int mission_start_value,const double scenario_time);

        /**
         * @brief Run decision making process for the current time
         * 
         * @param time current time
         */
        void Run(double time);

        /**
         * @brief Input traffic alert data
         * 
         * @param alert 
         */
        void InputTrafficAlert(std::string, int alert);

        /**
         * @brief internal state
         * 
         */
        CognitionState_t cogState;

    private:
        EventManagement<CognitionState_t> eventMng;
        void InitializeState();
        void InitializeEvents();
        void InitializeEventHandlers();

};

/**
 * @brief Get the pointer for given plan id from a list of plans
 * 
 * @param planList list of plans
 * @param plan_id  query plan id
 * @return larcfm::Plan*  pointer to queried plan
 */
larcfm::Plan* GetPlan(std::list<larcfm::Plan>* planList,const std::string &plan_id);

/**
 * @brief Get total Waypoints
 * 
 * @param planList list of plans
 * @param plan_id  query plan id
 * @return int total waypoints in queried plan
 */
int GetTotalWaypoints(std::list<larcfm::Plan>* planList,const std::string &plan_id);

/**
 * @brief Get next waypoint in given plan
 * 
 * @param fp plan
 * @param nextWP next waypoint index
 * @return larcfm::Position waypoint position
 */
larcfm::Position GetNextWP(larcfm::Plan*fp, int nextWP);

/**
 * @brief Get prev waypoint in given plan
 * 
 * @param fp plan
 * @param prevWP previous waypoint index
 * @return larcfm::Position previous waypoint position
 */
larcfm::Position GetPrevWP(larcfm::Plan*fp, int nextWP);

/**
 * @brief Get velocity of next waypoint
 * 
 * @param fp plan
 * @param nextWP waypoint index
 * @return larcfm::Velocity velocity at given waypoint index
 */
larcfm::Velocity GetNextWPVelocity(larcfm::Plan*fp, int nextWP);

/**
 * @brief check projected traffic conflict given lookahead, current position and velocity
 * 
 * @param position current position
 * @param velocity current velocity
 * @param prevWP previous waypoint position
 * @param nextWP next waypoint position
 * @param timeToViolation lookahead time
 * @param DTHR well clear radius 
 * @param ZTHR well clear height
 * @return true if conflict 
 */
bool CheckProjectedTrafficConflict(larcfm::Position position, larcfm::Velocity velocity,
                                   larcfm::Position prevWP,larcfm::Position nextWP,
                                   double timeToViolation,double DTHR,double ZTHR);

/**
 * @brief Compute feasibility of a given target position 
 * 
 * @param state decision making data
 * @param target target position
 */
bool ComputeTargetFeasibility(CognitionState_t *state,larcfm::Position target);

/**
 * @brief Create an output velocity command on the output cognitionCommands list
 * 
 * @param state decision making information
 * @param track heading
 * @param gs ground speed
 * @param vs vertical speed
 */
void SetGuidanceVelCmd(CognitionState_t *state,const double track,const double gs,const double vs);

/**
 * @brief Create an output speed command on the output cognitionCommands list
 * 
 * @param state decision making information
 * @param planID id of plan on which speed change is done
 * @param speed value of speed
 * @param hold true if speed change is to apply to entire flightplan, false if only for current leg
 */
void SetGuidanceSpeedCmd(CognitionState_t *state,const std::string &planID,const double speed,const int hold = 0);

/**
 * @brief Set the Guidance Alt for given plan on the output cognitionCommands list
 * 
 * @param state decision making information
 * @param planID id of plan on which alt change is done
 * @param alt value of altitude
 * @param hold true if alt change is to apply to entire flightplan, false if only for current leg
 */
void SetGuidanceAltCmd(CognitionState_t *state,const std::string &planID,const double alt,const int hold = 0);

/**
 * @brief enqueue the Flight Plan to be flown by guidance
 * 
 * @param state decision making information
 * @param plan_id id of plan
 * @param wp_index index of waypoint to which guidance should be started
 */
void SetGuidanceFlightPlan(CognitionState_t *state,const std::string &plan_id,const int wp_index);

/**
 * @brief enqueue the land command
 * 
 * @param state decision making state
 */
void SetLandCmd(CognitionState_t* state);

/**
 * @brief enqueue the ditch site request command
 * 
 * @param state 
 */
void SetDitchSiteRequestCmd(CognitionState_t* state);

/**
 * @brief enqueue a request to compute a new flightplan 
 * 
 * @param state 
 * @param planID name of plan
 * @param positionA starting position
 * @param velocityA starting velocity
 * @param positionB goal position
 * @param velocityB goal velocity
 */
void FindNewPath(CognitionState_t *state,const std::string &planID,const larcfm::Position &positionA,
                const larcfm::Velocity &velocityA,
                const larcfm::Position &positionB,
                const larcfm::Velocity &velocityB);

/**
 * @brief enqueue the point 2 point guidance command
 * 
 * @param state
 * @param point position of final point 
 * @param speed speed during the point2point segment
 */
void SetGuidanceP2P(CognitionState_t *state,const larcfm::Position &point,const double speed);

/**
 * @brief enqueue a status message in the output
 * 
 * @param state 
 * @param buffer string buffer
 * @param severity severity of message
 */
void SendStatus(CognitionState_t *state,const char buffer[],const uint8_t severity);

/**
 * @brief Get the Resolution Type object
 * 
 * @param state 
 * @return resolutionType_e 
 */
resolutionType_e GetResolutionType(CognitionState_t* state);

/**
 * @brief 
 * 
 * @param state 
 * @param message 
 */
void LogMessage(CognitionState_t* state,std::string message);

/**
 * @brief compute perpendicular deviation from a flightplan segement
 * 
 * @param prev_wp starting waypoint of segment
 * @param next_wp ending waypoint of segment
 * @param pos current position
 * @param offset [output] longitudinal and lateral offset
 * @return double 
 */
double ComputeXtrackDistance(const larcfm::Position &prev_wp,
                            const larcfm::Position &next_wp,
                            const larcfm::Position &pos,
                            double offset[]);

/**
 * @brief Get the nearest position on plan 
 * 
 * @param fp pointer to flightplan
 * @param current_pos current position
 * @param nextWP next waypoint
 * @return larcfm::Position closest point on flightplan
 */
larcfm::Position GetNearestPositionOnPlan(const larcfm::Plan* fp,
                                          const larcfm::Position &current_pos,int& nextWP);

/**
 * @brief Check if turning from old heading to new heading is conflict free
 * 
 * @param low min value of conflict band
 * @param high max value of conflict band
 * @param new_heading new heading
 * @param old_heading old heading
 * @param rightTurnConflict [out] true if right turn feasible
 * @param leftTurnConflict [out] true if left turn feasible
 * @return true if both turns are safe 
 */
bool CheckTurnConflict(const double low,
                    const double high,
                    const double new_heading,
                    const double old_heading,bool& rightTurnConflict,bool& leftTurnConflict);

/**
 * @brief Get the priority values from parameter object
 * 
 * @param params 
 * @return std::map<std::string,int> mapping between event names and priority values
 */
std::map<std::string,int> GetPriorityValues(cognition_params_t* params);

#endif
