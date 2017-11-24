#ifndef FLIGHTDATAWRAPPER_H 
#define FLIGHTDATAWRAPPER_H 

#ifdef __cplusplus
extern "C"{
#endif

struct c_FlightData;

struct c_FlightData* c_initFlightData();

void c_AddMissionItem(struct c_FlightData* cptr,waypoint_t* msg);

void c_AddResolutionItem(struct c_FlightData* cptr,waypoint_t* msg);

void c_SetStartMissionFlag(struct c_FlightData* cptr,uint8_t flag);

void c_ConstructMissionPlan(struct c_FlightData* cptr);

void c_ConstructResolutionPlan(struct c_FlightData* cptr);

void c_InputState(struct c_FlightData* cptr,double time,double lat,double lon,double alt,double vx,double vy,double vz);

void c_AddTraffic(struct c_FlightData* cptr,int id,double x,double y,double z,double vx,double vy,double vz);

void c_GetTraffic(struct c_FlightData* cptr,int id,double* x,double* y,double* z,double* vx,double* vy,double* vz);

void c_ClearMissionList(struct c_FlightData* cptr);

void c_ClearResolutionList(struct c_FlightData* cptr);

void c_InputNextMissionWP(struct c_FlightData* cptr,int index);

void c_InputNextResolutionWP(struct c_FlightData* cptr,int index);

void c_Reset(struct c_FlightData* cptr);

void c_InputAck(struct c_FlightData* cptr,CmdAck_t* ack);

bool c_CheckAck(struct c_FlightData* cptr,command_name_t command);

int8_t c_GetStartMissionFlag(struct c_FlightData* cptr);

uint16_t c_GetMissionPlanSize(struct c_FlightData* cptr);

uint16_t c_GetResolutionPlanSize(struct c_FlightData* cptr);

uint16_t c_GetNextMissionWP(struct c_FlightData* cptr);

uint16_t c_GetNextResolutionWP(struct c_FlightData* cptr);

#ifdef __cplusplus 
};
#endif

#endif