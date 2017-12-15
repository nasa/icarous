#include "FlightData.h"
#include "FlightDataWrapper.h"

#define ToCPP(a) (reinterpret_cast<FlightData*>(a))
#define ToC(a) (reinterpret_cast<c_FlightData*>(a))


struct c_FlightData* c_initFlightData(char configfile[]){
    FlightData* ccptr = new FlightData(configfile);
    return ToC(ccptr);
}

void c_AddMissionItem(struct c_FlightData* cptr,waypoint_t* msg){
    FlightData* ccptr = ToCPP(cptr);
    return ccptr->AddMissionItem(msg);
}

void c_AddResolutionItem(struct c_FlightData* cptr,waypoint_t* msg){
    FlightData* ccptr = ToCPP(cptr);
    return ccptr->AddResolutionItem(msg);
}

void c_SetStartMissionFlag(struct c_FlightData* cptr,uint8_t flag){
    FlightData* ccptr = ToCPP(cptr);
    return ccptr->SetStartMissionFlag(flag);
}

void c_ConstructMissionPlan(struct c_FlightData* cptr){
    FlightData* ccptr = ToCPP(cptr);
    return ccptr->ConstructMissionPlan();
}

void c_ConstructResolutionPlan(struct c_FlightData* cptr){
    FlightData* ccptr = ToCPP(cptr);
    return ccptr->ConstructResolutionPlan();
}

void c_InputState(struct c_FlightData* cptr,double time,double lat,double lon,double alt,double vx,double vy,double vz){
    FlightData* ccptr = ToCPP(cptr);
    return ccptr->InputState(time,lat,lon,alt,vx,vy,vz);
}

void c_AddTraffic(struct c_FlightData* cptr,int id,double x,double y,double z,double vx,double vy,double vz){
    FlightData* ccptr = ToCPP(cptr);
    return ccptr->AddTraffic(id,x,y,z,vx,vy,vz);
}

void c_GetTraffic(struct c_FlightData* cptr,int id,double* x,double* y,double* z,double* vx,double* vy,double* vz){
    FlightData* ccptr = ToCPP(cptr);
    return ccptr->GetTraffic(id,x,y,z,vx,vy,vz);
}

void c_ClearMissionList(struct c_FlightData* cptr){
    FlightData* ccptr = ToCPP(cptr);
    return ccptr->ClearMissionList();
}

void c_ClearResolutionList(struct c_FlightData* cptr){
    FlightData* ccptr = ToCPP(cptr);
    return ccptr->ClearResolutionList();
}

void c_InputNextMissionWP(struct c_FlightData* cptr,int index){
    FlightData* ccptr = ToCPP(cptr);
    return ccptr->InputNextMissionWP(index);
}

void c_InputNextResolutionWP(struct c_FlightData* cptr,int index){
    FlightData* ccptr = ToCPP(cptr);
    return ccptr->InputNextResolutionWP(index);
}

void c_InputTakeoffAlt(struct c_FlightData* cptr,double alt){
    FlightData* ccptr = ToCPP(cptr);
    return ccptr->InputTakeoffAlt(alt);
}

void c_InputCruisingAlt(struct c_FlightData* cptr,double alt){
    FlightData* ccptr = ToCPP(cptr);
    return ccptr->InputCruisingAlt(alt);
}

void c_InputGeofenceData(struct c_FlightData* cptr,geofence_t* gf){
    FlightData* ccptr = ToCPP(cptr);
    return ccptr->InputGeofenceData(gf);
}

double c_GetTakeoffAlt(struct c_FlightData* cptr){
    FlightData* ccptr = ToCPP(cptr);
    return ccptr->GetTakeoffAlt();
}

double c_GetCruisingAlt(struct c_FlightData* cptr){
    FlightData* ccptr = ToCPP(cptr);
    return ccptr->GetCruisingAlt();
}

double c_GetAltitude(struct c_FlightData* cptr){
    FlightData* ccptr = ToCPP(cptr);
    return ccptr->GetAltitude();
}

int c_GetTotalMissionWP(struct c_FlightData* cptr){
    FlightData* ccptr = ToCPP(cptr);
    return ccptr->GetTotalMissionWP();
}

int c_GetTotalResolutionWP(struct c_FlightData* cptr){
    FlightData* ccptr = ToCPP(cptr);
    return ccptr->GetTotalResolutionWP();
}

int c_GetTotalTraffic(struct c_FlightData* cptr){
    FlightData* ccptr = ToCPP(cptr);
    return ccptr->GetTotalTraffic();
}

void c_Reset(struct c_FlightData* cptr){
    FlightData* ccptr = ToCPP(cptr);
    return ccptr->Reset();
}

void c_InputAck(struct c_FlightData* cptr,CmdAck_t* ack){
    FlightData* ccptr = ToCPP(cptr);
    return ccptr->InputAck(ack);
}

bool c_CheckAck(struct c_FlightData* cptr,command_name_t command){
    FlightData* ccptr = ToCPP(cptr);
    return ccptr->CheckAck(command);
}

int8_t c_GetStartMissionFlag(struct c_FlightData* cptr){
    FlightData* ccptr = ToCPP(cptr);
    return ccptr->GetStartMissionFlag();
}

uint16_t c_GetMissionPlanSize(struct c_FlightData* cptr){
    FlightData* ccptr = ToCPP(cptr);
    return ccptr->GetMissionPlanSize();
}

uint16_t c_GetResolutionPlanSize(struct c_FlightData* cptr){
    FlightData* ccptr = ToCPP(cptr);
    return ccptr->GetResolutionPlanSize();
}

uint16_t c_GetNextMissionWP(struct c_FlightData* cptr){
    FlightData* ccptr = ToCPP(cptr);
    return ccptr->GetNextMissionWP();
}

uint16_t c_GetNextResolutionWP(struct c_FlightData* cptr){
    FlightData* ccptr = ToCPP(cptr);
    return ccptr->GetNextResolutionWP();
}

int c_GetTotalFences(struct c_FlightData* cptr){
    FlightData* ccptr = ToCPP(cptr);
    return ccptr->GetTotalFences();
}

