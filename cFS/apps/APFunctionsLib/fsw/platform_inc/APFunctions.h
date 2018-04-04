//
// Created by Swee Balachandran on 11/28/17.
//

#ifndef APFUNCTIONS_H
#define APFUNCTIONS_H

#include "cfe.h"
#include "msgids/msgids.h"
#include "Icarous_msg.h"

#define InitCmdMsg(cmd)\
ArgsCmd_t cmd; \
CFE_SB_InitMsg(&cmd,AP_COMMANDS_MID,sizeof(ArgsCmd_t),TRUE);

#define InitTrafficServiceMsg(msg)\
service_t msg; \
CFE_SB_InitMsg(&msg,SERVICE_TRAFFIC_MID,sizeof(service_t),TRUE);\
msg.id = (rand() + rand());

#define InitTrajectoryServiceMsg(msg)\
service_t msg; \
CFE_SB_InitMsg(&msg,SERVICE_TRAJECTORY_MID,sizeof(service_t),TRUE);\
msg.id = (rand() + rand());

#define InitGeofenceServiceMsg(msg)\
service_t msg; \
CFE_SB_InitMsg(&msg,SERVICE_GEOFENCE_MID,sizeof(service_t),TRUE);\
msg.id = (rand() + rand());

int32 APFunctionsLibInit(void);
void SetMode(icarous_control_mode_t mode);
void ArmThrottles(uint8_t c);
void SetNextMissionItem(int nextWP);
void Takeoff(float alt);
void Land();
void SetYaw(uint8_t relative,double heading);
void SetGPSPosition(double lat,double lon,double alt);
void SetVelocity(double Vn, double Ve, double Vu);
void SetSpeed(float speed);

// helper functions to request services from geofence app
int ServiceFence_GetFenceViolation(double* position,double* velocity);
int ServiceFence_GetWPFeasibility(double* positionA,double* positionB);
int ServiceFence_GetRecoveryPosition(double* position);

// helper function to request services from traffic app
int ServiceTraffic_GetTrafficConflict(bool bands,double* position,double* velocity);
int ServiceTraffic_GetSafe2Turn(double* position,double* velocity,double headingA,double headingB);

// helper functions to request services from trajectory app
int ServiceTrajectory_GetWaypoint(char* planID,int index);
int ServiceTrajectory_GetTotalWaypoints(char* planID);
int ServiceTrajectory_GetNewPath(char* planID,char* algorithm,double* positionA,double* velocity,double* positionB);
int ServiceTrajectory_GetXTrackDeviation(char* planID,int leg,double* position);
int ServiceTrajectory_GetDistance(double* positionA,double* positionB);
int ServiceTrajectory_GetExitPoint(char* planID,double* position,int nextWP);
int ServiceTrajectory_GetInterceptHeading(char* planID,int nextWP,double* position);
int ServiceTrajectory_GetInterceptManeuver(char* planID,int nextWP,double* position);

// helper functions to decode service response
void ServiceFence_DecodeFenceViolation(service_t* msg,bool* output);
void ServiceFence_DecodeWPFeasibility(service_t* msg,bool* output);
void ServiceFence_DecodeRecoveryPosition(service_t* msg,double* output);

void ServiceTraffic_DecodeTrafficConflict(service_t* msg,bool* conflict,double* output);
void ServiceTraffic_DecodeSafe2Turn(service_t* msg,bool* output);

void ServiceTrajectory_DecodeWaypoint(service_t* msg,double* output);
void ServiceTrajectory_DecodeTotalWaypoints(service_t* msg,int* output);
void ServiceTrajectory_DecodeNewPath(service_t* msg,int* output);
void ServiceTrajectory_DecodeXTrackDeviation(service_t* msg,double* output);
void ServiceTrajectory_DecodeDistance(service_t* msg,double* output);
void ServiceTrajectory_DecodeExitPoint(service_t* msg,double* output);
void ServiceTrajectory_DecodeInterceptHeading(service_t* msg,double* output);
void ServiceTrajectory_DecodeInterceptManeuver(service_t* msg,double* output);

#endif //APFUNCTIONS_H
