//
// Created by Swee Balachandran on 11/21/17.
//

#include "FlightData.h"
#include "FlightDataWrapper.h"

#define ToCPP(a) (reinterpret_cast<FlightData*>(a))
#define ToC(a)   (reinterpret_cast<flightData*>(a))

struct flightData* initilizeFlightData(){
    FlightData* fdata = new FlightData();
    return ToC(fd);
}

void c_InputState(double time,double lat,double lon,double alt,double vx,double vy,double vz){
    FlightData* fdata = ToCPP(fd);
    fdata->InputState(time,lat,lon,alt,vx,vy,vz);
}

void c_addMissionItem(struct flightData* fd,waypoint_t *msg){
    FlightData* fdata = ToCPP(fd);
    fdata->AddMissionItem(msg);
}

void c_addResolutionItem(struct flightData* fd,waypoint_t *msg){
    FlightData* fdata = ToCPP(fd);
    fdata->AddResolutionItem(msg);
}


void c_SetStartMissionFlag(struct flightData* fd,uint8_t flag){
    FlightData* fdata = ToCPP(fd);
    fdata->SetStartMissionFlag(flag);
}

void c_ConstructMissionPlan(struct flightData* fd){
    FlightData* fdata = ToCPP(fd);
    fdata->ConstructMissionPlan();
}

void c_ConstructResolutionPlan(struct flightData* fd){
    FlightData* fdata = ToCPP(fd);
    fdata->ConstructResolutionPlan();
}

void c_AddTraffic(struct flightData* fd,int id, double x,double y, double z, double vx,double vy,double vz){
    FlightData* fdata = ToCPP(fd);
    fdata->AddTraffic(id,x,y,z,vx,vy,vz);
}

void c_ClearMissionList(struct flightData* fd){
    FlightData* fdata = ToCPP(fd);
    fdata->ClearMissionList();
}

void c_ClearResolutionList(struct flightData* fd){
    FlightData* fdata = ToCPP(fd);
    fdata->ClearResolutionList();
}

void c_Reset(){
    FlightData* fdata = ToCPP(fd);
    fdata->Reset();
}

void c_InputAck(CmdAck_t* ack){
    FlightData* fdata = ToCPP(fd);
    fdata->InputAck(ack);
}

bool c_CheckAck(command_name_t command){
    FlightData* fdata = ToCPP(fd);
    return fdata->CheckAck(command);
}

int8_t c_GetStartMissionFlag(struct flightData* fd){
    FlightData* fdata = ToCPP(fd);
    return fdata->GetStartMissionFlag();
}

uint16_t c_GetMissionPlanSize(struct flightData* fd){
    FlightData* fdata = ToCPP(fd);
    return fdata->GetMissionPlanSize();
}

uint16_t c_GetResolutionPlanSize(struct flightData* fd){
    FlightData* fdata = ToCPP(fd);
    return fdata->GetResolutionPlanSize();
}
