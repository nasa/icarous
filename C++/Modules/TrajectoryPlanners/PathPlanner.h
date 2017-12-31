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
    int64_t FindPathAstar(char planID[],double fromPosition[],double toPosition[]);
    int64_t FindPathRRT(char planID[],double fromPosition[],double toPosition[],double trk,double gs,double vs);

public:
    PathPlanner(FlightData* fdata);
    int64_t FindPath(algorithm,char planID[], double fromPosition[],double toPosition[],double trk,double gs,double vs);
    int GetTotalWaypoints(char planID[]);
    void GetWaypoint(char planID[],int wpID,double waypoint[]);
    void InputFlightPlan(char planID[],int wpID,double waypoint[],double speed);
    double Dist2Waypoint(double currPosition[],double nextWaypoint[]);
    void OutputFlightPlan(ENUProjection* proj,char* planID,char* fenceFile,char* waypointFile);
    double ComputeXtrackDistance(char planID[],int leg,double position[],double offset[]);
};

#endif

#endif //PATHPLANNER_H
