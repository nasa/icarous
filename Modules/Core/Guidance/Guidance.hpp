/**
 * @file Guidance.hpp
 * @brief Contains the guidance functionality
 */

#ifndef GUIDANCE_HPP
#define GUIDANCE_HPP

#include <cstring>
#include <list>
#include <cmath>
#include <vector>
#include <iostream>
#include <Plan.h>
#include <Position.h>
#include <Velocity.h>
#include <TrajGen.h>

#include "Guidance.h"
#include "Interfaces.h"

/**
 * @brief Core class for performing guidance computations
 * 
 */
class Guidance{
private:

    std::list<larcfm::Plan> planList;                 ///< List of all plans received by guidance
    std::map<std::string,int> nextWpId;               ///< Map from flight plan id to next waypoint id
    std::string activePlanId;                         ///< Plan ID for active plan
    std::string prevPlan;                             ///< Plan ID for previous plan
    larcfm::Plan* currentPlan;                        ///< Pointer to current plan data structure
    larcfm::Position currentPos;                      ///< Current position
    larcfm::Velocity currentAirspeed;                 ///< Current velocity (airspeed)
    larcfm::Velocity currentGroundSpeed;              ///< Current velocity (ground speed)
    larcfm::Velocity outputCmd;                       ///< Output command computed by guidance
    GuidanceMode mode;                                ///< Current guidance mode
    double currTime;                                  ///< Current time
    bool wpReached;                                   ///< Flag indicating waypoint arrival
    bool etaControl;                                  ///< Flag indicating eta control
    bool inTurn;                                      ///< Flag indicating aircraft is in a turn
    double distH2nextWP;                              ///< Horizontal distance to next waypoint
    double distV2nextWP;                              ///< Vertical distance to next waypoint
    double xtrackDist;                                ///< Cross track deviation
    double prevTrackControllerTime;                   ///< Time at which last cycle of the controller was run
    double prevTrackControllerTarget;                 ///< Previous target command for the controller
    larcfm::Velocity wind;                            ///< Wind velocity (to)
    
    GuidanceParams_t params;                          ///< Guidance parameters

    /**
     * @brief Function to compute the intersection of the line of sight circle with current plan segment
     * 
     * @param fp  Pointer to flightplan
     * @param nextWP  next waypoint index (waypoint index the aircraft is currently flying to)
     * @param guidance_radius radius of the line of sight circle
     * @param pos [out] offset position on plan
     * @return double cross track deviation from nominal path
     */
    double ComputeOffSetPositionOnPlan(larcfm::Plan* fp,int nextWP,double guidance_radius,larcfm::Position &pos);

    /**
     * @brief Returns the intersection point between a circle of radius r and the line between wpA and wpB.
     * current position is the origin and wpA and wpB are defined with respect to origin.
     * 
     * @param wpA starting waypoint of segment
     * @param wpB ending waypoint of segment
     * @param r radius of line of sight circle
     * @return larcfm::Vect3 Intersection of circle with plan segment
     */
    larcfm::Vect3 GetCorrectIntersectionPoint(const larcfm::Vect3 &wpA,const larcfm::Vect3 &wpB,const double r);

    /**
     * @brief Compute distance given two points
     * 
     * @param x1 x coordinate
     * @param y1 y coordinate
     * @param x2 x coordinate
     * @param y2 y coordinate 
     * @return double distance
     */
    double distance(double x1,double y1,double x2,double y2);

    /**
     * @brief Compute current speed that should be used for output command 
     * 
     * @return double 
     */
    double ComputeSpeed();

    /**
     * @brief Compute current climb rate for the output
     * 
     * @param speedRef computed speed
     * @return double output climb rate
     */
    double ComputeClimbRate(double speedRef);

    /**
     * @brief Compute new heading 
     * 
     * @param speedRef computed speed
     * @return double output heading
     */
    double ComputeNewHeading(double& speedRef);

    /**
     * @brief A dot product computation to check if vehicle has crossed the waypoint. < 0 if crossed. >= 0 if not crossed
     * 
     * @param position current position
     * @param velocity current velocity
     * @param waypoint waypoint flying towards
     * @return double dot product value
     */
    double GetApproachPrecision(const larcfm::Position &position,const larcfm::Velocity &velocity, const larcfm::Position &waypoint);

    /**
     * @brief Get the Plan object
     * 
     * @param plan_id id of plan
     * @return larcfm::Plan* pointer to plan
     */
    larcfm::Plan* GetPlan(const std::string &plan_id);

    /**
     * @brief Filter command (rate limits and saturations)
     * 
     * @param refHeading output heading
     * @param refSpeed output speed
     * @param refVS output climb rate
     */
    void FilterCommand(double &refHeading, double &refSpeed, double &refVS);

    /**
     * @brief Perform guidance computation
     * 
     */
    void ComputePlanGuidance();

    /**
     * @brief Check waypoint arrival
     * 
     */
    void CheckWaypointArrival();
public:

    /**
     * @brief Construct a new Guidance object
     * 
     * @param config configuration filename
     */
    Guidance(const std::string config);

    /**
     * @brief Read parameters from file
     * 
     * @param config configuration filename
     */
    void ReadParamFromFile(std::string config);

    /**
     * @brief Set the Guidance Params object
     * 
     * @param params 
     */
    void SetGuidanceParams(const GuidanceParams_t* params);

    /**
     * @brief Set the Aircraft State 
     * 
     * @param pos current position
     * @param groundSpeed current ground speed
     */
    void SetAircraftState(const larcfm::Position &pos,const larcfm::Velocity &groundSpeed);

    /**
     * @brief Set the Wind Data 
     * 
     * @param windFrom wind from direction
     * @param windSpeed wind speed
     */
    void SetWindData(const double windFrom,const double windSpeed);

    /**
     * @brief Input flightplan
     * 
     * @param plan_id flightplan id
     * @param waypoints list of waypoints
     * @param initHeading initial path heading (only needed for repairing)
     * @param repair set to true to convert linear plan to kinematic plan using given turn rate for repair
     * @param repairTurnRate turn rate to use for repairing
     */
    void InputFlightplanData(const std::string &plan_id,const std::list<waypoint_t> &waypoints,
                            const double initHeading,bool repair,double repairTurnRate);

    /**
     * @brief Perform guidance computations for given time
     * 
     * @param time 
     * @return int 
     */
    int RunGuidance(double time);

    /**
     * @brief Set the Guidance Mode 
     * 
     * @param mode guidance mode
     * @param planID plan id
     * @param nextWP waypoint to which aircraft should start flying to
     * @param eta enable eta constraints
     */
    void SetGuidanceMode(const GuidanceMode mode,const std::string planID,const int nextWP,const bool eta);

    /**
     * @brief Change altitude in flightplan segment
     * 
     * @param planID id of plan
     * @param wpID index of waypoint for which altitude must be changed
     * @param value value of new altitude
     * @param updateAll update altitude for entire plan from wpID
     */
    void ChangeWaypointAlt(const std::string planID,int wpID,const double value,const bool updateAll);

    /**
     * @brief Change speed in flightplan segement
     * 
     * @param planID if of plan
     * @param wpID index of waypoint for which speed must be changed
     * @param value value of new speed
     */
    void ChangeWaypointSpeed(const std::string planID,int wpID,const double value);


    /**
     * @brief Change eta constraints in flightplan
     * 
     * @param planID ID of plan
     * @param wpID index of waypoint at which eta should change
     * @param value value of new eta
     * @param updateAll update eta for rest of flightplan
     */
    void ChangeWaypointETA(const std::string planID,const int wpID,const double value,const bool updateAll);

    /**
     * @brief Set the Velocity Commands (used when an external modules is computing guidance commands)
     * 
     * @param inputs input velocity commands
     */
    void SetVelocityCommands(const larcfm::Velocity &inputs);

    /**
     * @brief Get the Output object
     * 
     * @param output 
     */
    void GetOutput(GuidanceOutput_t& output);

    /**
     * @brief Get Waypoint 
     * 
     * @param planID id of plan
     * @param id waypoint index
     * @param wp [out] waypoint object
     * @return int 
     */
    int GetWaypoint(const std::string planID,int id,waypoint_t &wp);
};


#endif