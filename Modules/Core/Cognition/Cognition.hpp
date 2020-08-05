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


#include "Interfaces.h"
#include "Position.h"
#include "Velocity.h"
#include "Units.h"
#include "Plan.h"

#include "Commands.hpp"

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
    TRACK_RESOLUTION2,
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

class Cognition{
    public:
        Cognition(const std::string callSign);

        void Reset();

        void ResetFlightPhases();

        void InputVehicleState(const larcfm::Position &pos,const larcfm::Velocity &vel,const double heading);

        void InputFlightPlanData(const std::string &plan_id,
                                 const double scenario_time,
                                 const int wp_id,
                                 const larcfm::Position &wp_position,
                                 const int wp_metric,
                                 const double wp_value);

        void InputParameters(const cognition_params_t &new_params);

        void InputDitchStatus(const larcfm::Position &ditch_site,const bool ditch_requested);

        void InputMergeStatus(const int merge_status);

        void InputTrackBands(const bands_t &track_bands);

        void InputSpeedBands(const bands_t &speed_bands);

        void InputAltBands(const bands_t &alt_bands);

        void InputVSBands(const bands_t &vs_bands);

        void InputGeofenceConflictData(const geofenceConflict_t &gf_conflict);

        void ReachedWaypoint(const std::string &plan_id, const int wp_reached_id);

        int GetCognitionOutput(Command &command);

        void GetResolutionType();

        void StartMission(const int mission_start_value,const double scenario_time);

        /**
         * Top level finite state machines governing flight phase transitions
         */
        int FlightPhases(double time);

    private:
        void Initialize();

        larcfm::Plan* GetPlan(const std::string &plan_id);

        int GetTotalWaypoints(const std::string &plan_id);

        larcfm::Position GetNextWP();

        larcfm::Position GetPrevWP();

        bool NextWpFeasible();

        bool CheckPlanComplete(const std::string &plan_id);

        bool TrafficConflictManagement();

        bool GeofenceConflictManagement();

        bool ReturnToNextWP();

        bool XtrackManagement();

        bool TimeManagement();

        status_e Takeoff();

        status_e Climb();

        status_e Cruise();

        status_e Descent();

        status_e Approach();

        status_e Landing();

        status_e EmergencyDescent();

        bool RunTrafficResolution();

        bool CheckSafeToTurn(const double from_heading,const double to_heading);

        void SetGuidanceVelCmd(const double track,const double gs,const double vs);

        void SetGuidanceSpeedCmd(const std::string &planID,const double speed,const int hold = 0);

        void SetGuidanceFlightPlan(const std::string &plan_id,const int wp_index);

        void FindNewPath(const std::string &planID,const larcfm::Position &positionA,
                         const larcfm::Velocity &velocityA,
                         const larcfm::Position &positionB);

        void SetGuidanceP2P(const larcfm::Position &point,const double speed);

        void SendStatus(const char buffer[],const uint8_t severity);

        // Cognition util functions
        double ComputeHeading(const larcfm::Position &positionA,const larcfm::Position &positionB);
        double ComputeXtrackDistance(const larcfm::Position &prev_wp,
                                     const larcfm::Position &next_wp,
                                     const larcfm::Position &pos,
                                     double offset[]);
        void GetPositionOnPlan(const larcfm::Position &prev_wp,
                               const larcfm::Position &next_wp,
                               const larcfm::Position &current_pos,
                               larcfm::Position &position_on_plan);
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

    private:
        double utcTime;                         ///< Current time
        std::string timeString;                 ///< Formatted time string
        std::string callSign;                   ///< Vehicle's callsign string
        cognition_params_t parameters;          ///< Configurable parameters used by Cognition

        // Flight plan book keeping
        std::list<larcfm::Plan> flightPlans;        ///< List of flight plans
        larcfm::Plan* activePlan;                   ///< Pointer to the active flight plan
        std::map<std::string,int> nextWpId;         ///< Map from flight plan id to next waypoint id

        std::map<std::string,int> nextFeasibleWpId;             ///< Map from flight plan id to next feasible waypoint id
        std::map<std::string,std::vector<bool>> wpFeasibility;  ///< Map from flight plan id to vector of wp feasibility
        std::map<std::string,std::vector<bool>> wpMetricEta;    ///< Map from flight plan id to vector of wp metric type
        bool closestPointFeasible;                              ///< Feasibility of nearest point on primary flight plan

        bool primaryFPReceived;
        double scenarioTime;

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

        // Traffic conflict related variables
        bool trafficConflict;
        bool trafficTrackConflict;
        bool trafficSpeedConflict;
        bool trafficAltConflict;

        bool returnSafe;

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

        // Flight plan monitoring related variables
        bool XtrackConflict;
        bool directPathToFeasibleWP1;
        bool directPathToFeasibleWP2;

        double xtrackDeviation;

        // Ditching variables
        larcfm::Position ditchSite;
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
        bool p2pComplete;
        bool topOfDescent;

        // Status of merging activity
        uint8_t mergingActive;
        uint16_t numSecPaths;

        // output
        std::list<Command> cognitionCommands;

        std::ofstream log;
};

#endif
