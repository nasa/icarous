#ifndef TRAJMANAGERC_H
#define TRAJMANAGERC_H
#include "DubinsParams.h"
#include "Interfaces.h"

#ifdef __cplusplus
extern "C" {
#endif

void* new_TrajManager(char callsign[],char config[]);
void TrajManager_ReadParamFromFile(void* obj,char config[]);
void TrajManager_UpdateDubinsPlannerParameters(void * obj,DubinsParams_t* params);
int TrajManager_FindPath(void * obj,char planID[], double fromPosition[],double toPosition[],double fromVelocity[],double toVelocity[]);
int TrajManager_GetTotalWaypoints(void * obj, char * planid);
int TrajManager_GetWaypoint(void * obj, char * planid, int id,  waypoint_t * wp);
void TrajManager_ClearAllPlans(void * obj);
void TrajManager_InputGeofenceData(void * obj,int type,int index, int totalVertices, double floor, double ceiling, double pos[][2]);
int TrajManager_InputTraffic(void* obj,char* callsign, double *position, double *velocity,double time);
void TrajManager_InputFlightPlan(void* obj, char planID[],waypoint_t wpts[],int totalwp,
                                 double initHeading,bool kinematize,double repairTurnRate);
void TrajManager_CombinePlan(void* obj,char planID_A[],char planID_B[],int index);
void TrajManager_PlanToString(void* obj,char planID[],char outputString[],bool tcpColumnsLocal,long int timeshift);
void TrajManager_StringToPlan(void* obj,char planID[],char inputString[]);
void TrajManager_SetPlanOffset(void*obj, char planID[],int n,double offset);
trajectoryMonitorData_t TrajManager_MonitorTrajectory(void* obj,double time,char planID[],double position[],double velocity[],int nextWP1,int nextWP2);

#ifdef __cplusplus
}
#endif

#endif