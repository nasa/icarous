//
// Created by Swee Balachandran on 12/14/17.
//

#ifndef PATHPLANNER_H
#define PATHPLANNER_H

#include "ParameterData.h"
#include "FlightData.h"

typedef enum algType{
    _ASTAR_,_RRT_
}algorithm;

using namespace larcfm;

class PathPlanner{

private:
    FlightData* fdata;
    std::list<Plan> outputPlans;
    Plan ComputeGoAbovePlan(Position start,Position goal,double altFence,double rSpeed);
    int64_t FindPathAstar(char planID[],double fromPosition[],double toPosition[]);
    int64_t FindPathRRT(char planID[],double fromPosition[],double toPosition[],double trk,double gs,double vs);

public:
    PathPlanner(FlightData* fdata);
    int64_t FindPath(algorithm,char planID[], double fromPosition[],double toPosition[],double trk,double gs,double vs);
    void GetWaypoint(char planID[],int wpID,double waypoint[]);


};


#endif //PATHPLANNER_H
