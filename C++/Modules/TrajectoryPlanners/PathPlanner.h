//
// Created by Swee Balachandran on 12/14/17.
//

#ifndef PATHPLANNER_H
#define PATHPLANNER_H

typedef enum algorithm{
    _ASTAR_,_RRT_
}algorithm;

#ifdef __cplusplus

#include "ParameterData.h"
#include "FlightData.h"

class PathPlanner{

private:
    FlightData* fdata;
    std::list<Plan> flightPlans;
    Plan ComputeGoAbovePlan(Position start,Position goal,double altFence,double rSpeed);
    Plan* GetPlan(char planID[]);
    int64_t FindPathAstar(char planID[],double fromPosition[],double toPosition[]);
    int64_t FindPathRRT(char planID[],double fromPosition[],double toPosition[],double velocity[]);
public:
    PathPlanner(FlightData* fdata);
    int FindPath(algorithm,char planID[], double fromPosition[],double toPosition[],double velocity[]);
    int GetTotalWaypoints(char planID[]);
    void GetWaypoint(char planID[],int wpID,double waypoint[]);
    void InputFlightPlan(char planID[],int wpID,double waypoint[],double speed);
    double Dist2Waypoint(double currPosition[],double nextWaypoint[]);
    void OutputFlightPlan(ENUProjection* proj,char* planID,char* fenceFile,char* waypointFile);

    double ComputeXtrackDistance(Plan* fp,int leg,double position[],double offset[]);
    double ComputeXtrackDistance_c(char planID[],int leg,double position[],double offset[]);
    void GetPositionOnPlan(Plan* fp,int leg,double currentPos[],double position[]);
    void GetPositionOnPlan_c(char planID[],int leg,double currentPos[],double position[]);
    double GetInterceptHeadingToPlan(Plan* fp,int leg,double currentPos[]);
    double GetInterceptHeadingToPlan_c(char planID[],int leg,double currentPos[]);
    void ManueverToIntercept(Plan* fp,int leg,double currPosition[],double velocity[]);
    void ManueverToIntercept_c(char* planID,int leg,double currPosition[],double velocity[]);
};

#endif

#endif //PATHPLANNER_H
