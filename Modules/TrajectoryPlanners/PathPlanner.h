//
// Created by Swee Balachandran on 12/14/17.
//

#ifndef PATHPLANNER_H
#define PATHPLANNER_H

typedef enum algorithm{
    _GRID_,_ASTAR_,_RRT_,_SPLINES_
}algorithm;

#ifdef __cplusplus

#include "ParameterData.h"
#include "CDIIPolygon.h"
#include "fence.h"
#include "GenericObject.h"
#include <list>

class PathPlanner{

private:
    double obsbuffer;
    double maxCeiling;

    // astar specific parameters
    bool _astar_enable3D;
    double _astar_gridSize;
    double _astar_resSpeed;
    double _astar_lookahead;
    double _astar_maxCeiling;
    string _astar_daaConfig;

    // RRT specific parameters
    double _rrt_resSpeed;
    int _rrt_maxIterations;
    double _rrt_dt;
    int _rrt_macroSteps;
    double _rrt_goalCaptureRadius;
    string _rrt_daaConfig;
    string daaParameters;

    // Bsplines planner specific parameters
    bool _bsplines_enable3D;
    double _bsplines_compTime;
    int _bsplines_numControlPts;
    int _bsplines_lenTVec;
    double _bsplines_knotVec[100];

    std::list<Plan> flightPlans;
    std::list<fence> fenceList;
    std::list<GenericObject> trafficList;
    CDPolycarp geoPolyCarp;
    CDIIPolygon geoCDIIPolygon;
    PolyPath geoPolyPath;
    Plan ComputeGoAbovePlan(Position start,Position goal,double altFence,double rSpeed);
    int64_t FindPathGridAstar(char planID[],double fromPosition[],double toPosition[]);
    int64_t FindPathAstar(char planID[],double fromPosition[],double toPosition[],double velocity[]);
    int64_t FindPathRRT(char planID[],double fromPosition[],double toPosition[],double velocity[]);
#ifdef SPLINES
    int64_t FindPathBSplines(char planID[],double fromPosition[],double toPosition[],double velocity[]);
#endif
public:
    PathPlanner(double obsBuffer,double maxCeiling);
    void InitializeAstarParameters(bool enable3D,double gridSize,double resSpeed,double lookahead,char daaConfig[]);
    void InitializeRRTParameters(double resSpeed,int Nsteps,double dt,int Dt,double capR,char daaConfig[]);
    void InitializeBSplinesParameters(bool enable3D,double computationT,int numControlPts,int lenTVec);
    void UpdateAstarParameters(bool enable3D,double gridSize,double resSpeed,double lookahead,char daaConfig[]);
    void UpdateRRTParameters(double resSpeed,int Nsteps,double dt,int Dt,double capR,char daaConfig[]);
    void UpdateDAAParameters(char parameterString[]);
    int FindPath(algorithm,char planID[], double fromPosition[],double toPosition[],double velocity[]);
    int GetTotalWaypoints(char planID[]);
    void GetWaypoint(char planID[],int wpID,double waypoint[]);
    void OutputFlightPlan(ENUProjection* proj,char* planID,char* fenceFile,char* waypointFile);


    Plan* GetPlan(char planID[]);
    void ClearAllPlans();
    void InputGeofenceData(int type,int index, int totalVertices, double floor, double ceiling, double pos[][2]);
    fence* GetGeofence(int id);
    void ClearFences();
    int InputTraffic(int id, double *position, double *velocity);

    void PlanToString(char planID[],char outputString[],bool tcpColumnsLocal,long int timeshift);
    void StringToPlan(char planID[],char inputString[]);
    void CombinePlan(char planID_A[],char planID_B[],int index);

    void InputFlightPlan(char planID[],int wpID,double waypoint[],double time);
    double Dist2Waypoint(double currPosition[],double nextWaypoint[]);
    double ComputeXtrackDistance(Plan* fp,int leg,double position[],double offset[]);
    double ComputeXtrackDistance_c(char planID[],int leg,double position[],double offset[]);
    void GetPositionOnPlan(Plan* fp,int leg,double currentPos[],double position[]);
    void GetPositionOnPlan_c(char planID[],int leg,double currentPos[],double position[]);
    double GetInterceptHeadingToPlan(Plan* fp,int leg,double currentPos[]);
    double GetInterceptHeadingToPlan_c(char planID[],int leg,double currentPos[]);
    double GetInterceptHeadingToPoint(double positionA[],double positionB[]);
    void ManueverToIntercept(Plan* fp,int leg,double currPosition[],double velocity[],double xtrkDevGain,double resolutionSpeed,double allowedDev);
    void ManueverToIntercept_c(char* planID,int leg,double currPosition[],double velocity[],double xtrkDevGain,double resolutionSpeed,double allowedDev);
    void GetExitPoint(char* planID,double currentPoisition[],int nextWP,double exitPosition[]);
    double GetApproxElapsedPlanTime(Plan* fp,double currentPos[],int nextWP);
};

#endif

#endif //PATHPLANNER_H
