#ifndef PATHPLANNERC_H
#define PATHPLANNERC_H

#ifdef __cplusplus
extern "C" {
#endif

void* new_PathPlanner(char callsign[]);
void PathPlanner_InitializeAstarParameters(void * obj,bool enable3D,double gridSize,double resSpeed,double lookahead,char daaConfig[]);
void PathPlanner_InitializeRRTParameters(void * obj,double resSpeed,int Nsteps,double dt,int Dt,double capR,char daaConfig[]);

#ifdef SPLINES
void PathPlanner_InitializeBSplinesParameters(void * obj,bool enable3D,double computationT,int numControlPts,int lenTVec);
#endif
void PathPlanner_UpdateAstarParameters(void * obj,bool enable3D,double gridSize,double resSpeed,double lookahead,char daaConfig[] );
void PathPlanner_UpdateRRTParameters(void * obj,double resSpeed,int Nsteps,double dt,int Dt,double capR,char daaConfig[]);
void PathPlanner_UpdateDAAParameters(void * obj,char parameterString[]);
int PathPlanner_FindPath(void * obj,unsigned int algorithm,char planID[], double fromPosition[],double toPosition[],double velocity[]);
int PathPlanner_GetTotalWaypoints(void * obj, char * planid);
void PathPlanner_GetWaypoint(void * obj, char * planid, int id,  double * wp);
void PathPlanner_ClearAllPlans(void * obj);
void PathPlanner_InputGeofenceData(void * obj,int type,int index, int totalVertices, double floor, double ceiling, double pos[][2]);
int PathPlanner_InputTraffic(void* obj,int id, double *position, double *velocity);
void PathPlanner_InputFlightPlan(void* obj,char planID[],int wpID,double waypoint[],double time);
void PathPlanner_CombinePlan(void* obj,char planID_A[],char planID_B[],int index);
void PathPlanner_PlanToString(void* obj,char planID[],char outputString[],bool tcpColumnsLocal,long int timeshift);
void PathPlanner_StringToPlan(void* obj,char planID[],char inputString[]);

#ifdef __cplusplus
}
#endif

#endif