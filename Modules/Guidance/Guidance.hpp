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

#include "Guidance.h"

class Guidance{
private:

    std::list<larcfm::Plan> planList; 
    std::map<std::string,int> nextWpId;                    ///< Map from flight plan id to next waypoint id
    std::string activePlanId;

    std::map<std::string,std::vector<double>> wpSpeeds;    ///< Map from flight plan id to vector of values 
    larcfm::Plan* currentPlan;
    larcfm::Position currentPos;
    larcfm::Velocity currentVel;
    larcfm::Velocity outputCmd;
    GuidanceMode mode;
    double currTime;
    bool wpReached;
    double distH2nextWP;
    double distV2nextWP;
    double xtrackDist;
    

    // Guidance parameters
    GuidanceParams_t params;

    larcfm::Position ComputeOffSetPositionOnPlan(double speedRef,double& deviation);

    larcfm::Vect3 GetCorrectIntersectionPoint(const larcfm::Vect3 &wpA,const larcfm::Vect3 &wpB,const double r);

    double distance(double x1,double y1,double x2,double y2);

    double ComputeSpeed(const larcfm::NavPoint &nextPoint, double refSpeed = 0.0);

    double ComputeClimbRate(const larcfm::Position &position,const larcfm::Position &nextWaypoint,double speed);

    double GetApproachPrecision(const larcfm::Position &position,const larcfm::Velocity &velocity, const larcfm::Position &waypoint);
    larcfm::Plan* GetPlan(const std::string &plan_id);

    void ComputePlanGuidance();
public:
    Guidance(const GuidanceParams_t* params);

    void SetGuidanceParams(const GuidanceParams_t* params);

    void SetAircraftState(const larcfm::Position &pos,const larcfm::Velocity &vel);

    void InputFlightplanData(const std::string &plan_id,
                             const double scenario_time,
                             const int wp_id,
                             const larcfm::Position &wp_position,
                             const bool eta,
                             const double value);


    int RunGuidance(double time);
    
    void SetGuidanceMode(const GuidanceMode mode,const std::string planID,const int nextWP);
    void ChangeWaypointSpeed(const std::string planID,int wpID,const double value,const bool updateAll);
    void ChangeWaypointETA(const std::string planID,const int wpID,const double value,const bool updateAll);
    void SetVelocityCommands(const larcfm::Velocity &inputs);

    void GetOutput(GuidanceOutput_t& output);
};


#endif