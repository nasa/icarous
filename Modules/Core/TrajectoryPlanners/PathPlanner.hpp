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
    int numPlans;
    double obsbuffer;
    double maxCeiling;

    // astar specific parameters
    bool _astar_enable3D;
    double _astar_gridSize;
    double _astar_resSpeed;
    double _astar_lookahead;
    double _astar_maxCeiling;
    string _astar_daaConfig;

    double maxBankAngle;
    double maxHorAccel;
    double maxVertAccel;

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

    
    std::ofstream log;

    std::list<Plan> flightPlans;
    std::list<fence> fenceList;
    std::list<GenericObject> trafficList;
    CDPolycarp geoPolyCarp;
    CDIIPolygon geoCDIIPolygon;
    PolyPath geoPolyPath;
    Plan ComputeGoAbovePlan(Position start,Position goal,double altFence,double rSpeed);
    int64_t FindPathGridAstar(char planID[]);
    int64_t FindPathAstar(char planID[]);
    int64_t FindPathRRT(char planID[]);
    void LogInput();
    Position GetPositionFromLog(std::ifstream& fp);
    Velocity GetVelocityFromLog(std::ifstream& fp);
    int GetIntFromLog(std::ifstream& fp);
    double GetDoubleFromLog(std::ifstream& fp);
    std::string GetPlanFromLog(std::ifstream& fp);
    std::string GetStringFromLog(std::ifstream& fp);
#ifdef SPLINES
    int64_t FindPathBSplines(char planID[]);
#endif
public:
    Position startPos;
    Position endPos;
    Velocity startVel;
    algorithm search;

    PathPlanner(std::string callsign);
    void InitializeAstarParameters(bool enable3D,double gridSize,double resSpeed,double lookahead,char daaConfig[]);
    void InitializeRRTParameters(double resSpeed,int Nsteps,double dt,int Dt,double capR,char daaConfig[]);
#ifdef SPLINES
    void InitializeBSplinesParameters(bool enable3D,double computationT,int numControlPts,int lenTVec);
#endif
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
    void InputGeofenceData(int type,int index, int totalVertices, double floor, double ceiling, std::list<Position> &vertices);
    fence* GetGeofence(int id);
    void ClearFences();
    int InputTraffic(int id, double *position, double *velocity);
    int InputTraffic(int id, Position &position, Velocity &velocity);
    void InputDataFromLog(string filename);
    void PlanToString(char planID[],char outputString[],bool tcpColumnsLocal,long int timeshift);
    void StringToPlan(char planID[],char inputString[]);
    void CombinePlan(char planID_A[],char planID_B[],int index);

    void InputFlightPlan(char planID[],int wpID,double waypoint[],double time);
    double Dist2Waypoint(double currPosition[],double nextWaypoint[]);
    double GetInterceptHeadingToPoint(double positionA[],double positionB[]);
    void GetExitPoint(char* planID,double currentPoisition[],int nextWP,double exitPosition[]);
    double GetApproxElapsedPlanTime(Plan* fp,double currentPos[],int nextWP);
};

#endif

#endif //PATHPLANNER_H


