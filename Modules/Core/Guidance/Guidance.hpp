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

class Guidance{
private:

    std::list<larcfm::Plan> planList; 
    std::map<std::string,int> nextWpId;                    ///< Map from flight plan id to next waypoint id
    std::string activePlanId;
    std::string prevPlan;
    larcfm::Plan* currentPlan;
    larcfm::Position currentPos;
    larcfm::Velocity currentAirspeed;
    larcfm::Velocity currentGroundSpeed;
    larcfm::Velocity outputCmd;
    GuidanceMode mode;
    double currTime;
    bool wpReached;
    bool etaControl;
    bool inTurn;
    double distH2nextWP;
    double distV2nextWP;
    double xtrackDist;
    double prevTrackControllerTime;
    double prevTrackControllerTarget;
    larcfm::Velocity wind;
    
    

    // Guidance parameters
    GuidanceParams_t params;

    double ComputeOffSetPositionOnPlan(larcfm::Plan* fp,int nextWP,double guidance_radius,larcfm::Position &pos);

    /* 
     * GetCorrectIntersectionPoint 
     * Returns the intersection point between a circle of radius r and the line between wpA and wpB 
     * Assume current position as the origin.
     * wpA and wpB are defined with respect to the current position.
    */
    larcfm::Vect3 GetCorrectIntersectionPoint(const larcfm::Vect3 &wpA,const larcfm::Vect3 &wpB,const double r);

    double distance(double x1,double y1,double x2,double y2);

    double ComputeSpeed();

    double ComputeClimbRate(double speedRef);

    double ComputeNewHeading(double& speedRef);

    double ConstrainTurnRate(double targetHeading,double maxTurnRate);

    double GetApproachPrecision(const larcfm::Position &position,const larcfm::Velocity &velocity, const larcfm::Position &waypoint);
    larcfm::Plan* GetPlan(const std::string &plan_id);

    void FilterCommand(double &refHeading, double &refSpeed, double &refVS);

    void ComputePlanGuidance();

    void CheckWaypointArrival();
public:
    Guidance(const GuidanceParams_t* params);

    void SetGuidanceParams(const GuidanceParams_t* params);

    void SetAircraftState(const larcfm::Position &pos,const larcfm::Velocity &groundSpeed);

    void SetWindData(const double windFrom,const double windSpeed);

    void InputFlightplanData(const std::string &plan_id,const std::list<waypoint_t> &waypoints,
                            const double initHeading,bool repair,double repairTurnRate);

    int RunGuidance(double time);
    
    void SetGuidanceMode(const GuidanceMode mode,const std::string planID,const int nextWP,const bool eta);
    void ChangeWaypointAlt(const std::string planID,int wpID,const double value,const bool updateAll);
    void ChangeWaypointSpeed(const std::string planID,int wpID,const double value);
    void ChangeWaypointETA(const std::string planID,const int wpID,const double value,const bool updateAll);
    void SetVelocityCommands(const larcfm::Velocity &inputs);

    void GetOutput(GuidanceOutput_t& output);
    int GetWaypoint(const std::string planID,int id,waypoint_t &wp);
};


#endif