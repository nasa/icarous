//
// Created by research133 on 11/21/17.
//

#ifndef FLIGHTDATAWRAPPER_H
#define FLIGHTDATAWRAPPER_H


#ifdef __cplusplus
extern "C"{
#endif

struct flightData;

struct flightData* initilizeFlightData();
void c_InputState(double time,double lat,double lon,double alt,double vx,double vy,double vz);
void c_addMissionItem(struct flightData* fd,waypoint_t *msg);
void c_addResolutionItem(struct flightData* fd,waypoint_t *msg);
void c_SetStartMissionFlag(struct flightData* fd,uint8_t flag);
void c_ConstructMissionPlan(struct flightData* fd);
void c_ConstructResolutionPlan(struct flightData* fd);
void c_AddTraffic(struct flightData* fd,int id, double x,double y, double z, double vx,double vy,double vz);
void c_ClearMissionList(struct flightData* fd);
void c_ClearResolutionList(struct flightData* fd);
void c_Reset();
void c_InputAck(CmdAck_t* ack);
bool c_CheckAck(command_name_t command);


int8_t GetStartMissionFlag(struct flightData* fd);
uint16_t GetMissionPlanSize(struct flightData* fd);
uint16_t GetResolutionPlanSize(struct flightData* fd);



#ifdef __cplusplus
};
#endif

#endif //FLIGHTDATAWRAPPER_H
