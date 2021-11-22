//
// Created by Swee Balachandran on 12/14/17.
//

#ifndef TRAJMANAGER_H
#define TRAJMANAGER_H

#include "ParameterData.h"
#include "CDII.h"
#include "CDSI.h"
#include "CDIIPolygon.h"
#include "CDPolycarp.h"
#include "PolycarpDetection.h"
#include "Projection.h"
#include "EuclideanProjection.h"
#include "DubinsPlanner.hpp"
#include "Interfaces.h"
#include <list>
#include <map>

typedef struct{
    std::string callsign;
    int id;
    double time;
    larcfm::Position position;
    larcfm::Velocity velocity;
}pObject;

typedef struct{
    enum FENCE_TYPE {KEEP_IN, KEEP_OUT};
    int fenceType;
    int id;
    larcfm::SimplePoly polygon;
}fenceObject;

class TrajManager{

private:
    
    DubinsPlanner dbPlanner;
    double wellClearDistH;
    double wellClearDistV;
    int numPlans;
    std::ofstream log;
    std::list<larcfm::Plan> flightPlans;
    std::list<larcfm::Plan> trafficPlans;
    std::list<fenceObject> fenceList;
    std::map<std::string,pObject> trafficList;

    // Function to compute dubins path
    int64_t FindDubinsPath(std::string planID);

    // Function to compute projected time to violation to all fence edges
    double FindTimeToFenceViolation(larcfm::Poly3D polygon,larcfm::Vect3 so,larcfm::Velocity vel);

    // Compute cross track (perpendicular deviation), along track (longitudinal deviations) and time delays 
    // for a given plan, next waypoint index, current position, velocity and time
    std::vector<double> ComputePlanOffsets(const std::string planID,int nextWP,larcfm::Position currentPos,larcfm::Velocity currentVel,double timeAtPos);

    void LogInput();
public:
    larcfm::Position startPos;
    larcfm::Position endPos;
    larcfm::Velocity startVel;
    larcfm::Velocity endVel;

    TrajManager(std::string callsign,std::string config);
    void ReadParamFromFile(std::string config);
    void UpdateDubinsPlannerParameters(DubinsParams_t &params);
    int FindPath(std::string planID, larcfm::Position fromPosition,larcfm::Position toPosition,larcfm::Velocity fromVelocity,larcfm::Velocity toVelocity);
    int GetTotalWaypoints(std::string planID);
    int GetWaypoint(std::string planID,int wpid,waypoint_t& wp);


    larcfm::Plan* GetPlan(std::string planID);
    void ClearAllPlans();
    void ClearFences();
    void InputGeofenceData(int type,int index, int totalVertices, double floor, double ceiling, double pos[][2]);
    void InputGeofenceData(int type,int index, int totalVertices, double floor, double ceiling, std::vector<larcfm::Position> &vertices);
    void InputFlightPlan(const std::string &plan_id, const std::list<waypoint_t> &waypoints, 
                         const double initHeading,bool repair,double repairTurnRate);
    int InputTraffic(std::string callsign, larcfm::Position &position, larcfm::Velocity &velocity,double time);
    std::string PlanToString(std::string planID);
    void StringToPlan(std::string planID,std::string inputString);
    void CombinePlan(std::string planA,std::string planB,int index);
    void SetPlanOffset(std::string planID,int n,double offset);
    larcfm::Position GetPlanPosition(std::string planID,double t);
    trajectoryMonitorData_t MonitorTrajectory(double time, std::string planID, larcfm::Position pos,larcfm::Velocity vel, int nextWP1,int nextWP2);
};


#endif //PATHPLANNER_H


