#include "TrajManager.h"
#include "TrajManager.hpp"

void* new_TrajManager(char callsign[],char config[]){
   TrajManager* pp = new TrajManager(std::string(callsign),std::string(config));
   return (void*)pp;
}

void TrajManager_ReadParamFromFile(void* obj,char config[]){
   TrajManager* pp = (TrajManager*)obj;
   pp->ReadParamFromFile(std::string(config));
}

void TrajManager_UpdateDubinsPlannerParameters(void * obj,DubinsParams_t *params){
   TrajManager* pp = (TrajManager*)obj;
   pp->UpdateDubinsPlannerParameters(*params);
}

int TrajManager_FindPath(void * obj,char planID[], double fromPosition[],double toPosition[],double fromVelocity[],double toVelocity[]){
   TrajManager* pp = (TrajManager*)obj;
   larcfm::Position posA = larcfm::Position::makeLatLonAlt(fromPosition[0],"degree",fromPosition[1],"degree",fromPosition[2],"m");
   larcfm::Position posB = larcfm::Position::makeLatLonAlt(toPosition[0],"degree",toPosition[1],"degree",toPosition[2],"m");
   larcfm::Velocity velA = larcfm::Velocity::makeTrkGsVs(fromVelocity[0],"degree",fromVelocity[1],"m/s",fromVelocity[2],"m/s");
   larcfm::Velocity velB = larcfm::Velocity::makeTrkGsVs(toVelocity[0],"degree",toVelocity[1],"m/s",toVelocity[2],"m/s");
   return pp->FindPath(planID,posA,posB,velA,velB);
}

int TrajManager_GetTotalWaypoints(void * obj, char * planid){
   TrajManager* pp = (TrajManager*)obj;
   return pp->GetTotalWaypoints(planid);

}

void TrajManager_SetPlanOffset(void*obj, char planID[],int n,double offset){
   TrajManager* pp = (TrajManager*)obj;
   return pp->SetPlanOffset(planID,n,offset);
}

int TrajManager_GetWaypoint(void * obj, char * planid, int id,  waypoint_t * wp){
   TrajManager* pp = (TrajManager*)obj;
   return pp->GetWaypoint(planid, id,  *wp);
}

void TrajManager_ClearAllPlans(void * obj){
   TrajManager* pp = (TrajManager*)obj;
   pp->ClearAllPlans();
}

void TrajManager_InputGeofenceData(void * obj,int type,int index, int totalVertices, double floor, double ceiling, double pos[][2]){
   TrajManager* pp = (TrajManager*)obj;
   pp->InputGeofenceData(type,index, totalVertices, floor, ceiling, pos);

}

int TrajManager_InputTraffic(void* obj,char* callsign, double *position, double *velocity,double time){
   TrajManager* pp = (TrajManager*)obj;
   larcfm::Position pos = larcfm::Position::makeLatLonAlt(position[0],"degree",position[1],"degree",position[2],"m");
   larcfm::Velocity vel = larcfm::Velocity::makeTrkGsVs(velocity[0],"degree",velocity[1],"m/s",velocity[2],"m/s");
   return pp->InputTraffic(std::string(callsign), pos, vel,time);
}


void TrajManager_InputFlightPlan(void* obj, char planID[],waypoint_t wpts[],int totalwp,
                                 double initHeading,bool kinematize,double repairTurnRate){
   TrajManager* pp = (TrajManager*)obj;
   std::list<waypoint_t> waypoints(wpts,wpts+totalwp);
   pp->InputFlightPlan(std::string(planID),waypoints,initHeading,kinematize,repairTurnRate);
}


void TrajManager_CombinePlan(void* obj,char planID_A[],char planID_B[],int index){
   TrajManager* pp = (TrajManager*)obj;
   pp->CombinePlan(std::string(planID_A),std::string(planID_B),index);
}

void TrajManager_PlanToString(void* obj,char planID[],char outputString[],bool tcpColumnsLocal,long int timeshift){
   TrajManager* pp = (TrajManager*)obj;
   std::string output = pp->PlanToString(planID);
   strcpy(outputString,output.c_str());
}

void TrajManager_StringToPlan(void* obj,char planID[],char inputString[]){
   TrajManager* pp = (TrajManager*)obj;
   pp->StringToPlan(planID,inputString);
}

trajectoryMonitorData_t TrajManager_MonitorTrajectory(void* obj,double time,char planID[],double position[],double velocity[],int nextWP1,int nextWP2){
   TrajManager* pp = (TrajManager*)obj;
   larcfm::Position pos = larcfm::Position::makeLatLonAlt(position[0],"degree",position[1],"degree",position[2],"m");
   larcfm::Velocity vel = larcfm::Velocity::makeTrkGsVs(velocity[0],"degree",velocity[1],"m/s",velocity[2],"m/s");
   return pp->MonitorTrajectory(time,std::string(planID),pos,vel,nextWP1,nextWP2);
}
