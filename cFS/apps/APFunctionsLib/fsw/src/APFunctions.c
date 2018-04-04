//
// Created by Swee Balachandran on 11/28/17.
//

#include "APFunctions.h"
#include <time.h>
#include <stdlib.h>
#include <Icarous_msg.h>

int32 APFunctionsLibInit(void)
{

    OS_printf ("APFunctions Lib Initialized.\n");
    srand(time(0));
    return CFE_SUCCESS;

}

// Set Mode
void SetMode(icarous_control_mode_t mode){
    InitCmdMsg(cmd);
    cmd.name = _SETMODE_;
    cmd.param1 = mode;
    SendSBMsg(cmd);
}

// Arm Motors
void ArmThrottles(uint8_t c){
    InitCmdMsg(cmd);
    cmd.name = _ARM_;
    cmd.param1 = c;
    SendSBMsg(cmd);
}

// Set Next WP
void SetNextMissionItem(int nextWP){
    InitCmdMsg(cmd);
    cmd.name = _GOTOWP_;
    cmd.param1 = nextWP;
    SendSBMsg(cmd);
}

// Takeoff
void Takeoff(float alt){
    InitCmdMsg(cmd);
    cmd.name = _TAKEOFF_;
    cmd.param1 = alt;
    SendSBMsg(cmd);
}

// Land
void Land(){
    InitCmdMsg(cmd);
    cmd.name = _LAND_;
    cmd.param5 = 0;
    cmd.param6 = 0;
    cmd.param7 = 0;
    SendSBMsg(cmd);
}

// Set yaw
void SetYaw(uint8_t relative,double heading){
    InitCmdMsg(cmd);
    cmd.name = _SETYAW_;
    cmd.param1 = heading;
    cmd.param2 = 0;
    cmd.param3 = 1;
    cmd.param4 = relative;
    SendSBMsg(cmd);
}

// Set GPS Position
void SetGPSPosition(double lat,double lon,double alt){
    InitCmdMsg(cmd);
    cmd.name = _SETPOS_;
    cmd.param1 = lat;
    cmd.param2 = lon;
    cmd.param3 = alt;
    SendSBMsg(cmd);
}

// Set Velocity
void SetVelocity(double Vn, double Ve, double Vu){
    InitCmdMsg(cmd);
    cmd.name = _SETVEL_;
    cmd.param1 = Vn;
    cmd.param2 = Ve;
    cmd.param3 = Vu;
    SendSBMsg(cmd);
}

// Set Speed
void SetSpeed(float speed){
    InitCmdMsg(cmd);
    cmd.name = _SETSPEED_;
    cmd.param1 = speed;
    SendSBMsg(cmd);
}

int ServiceFence_GetFenceViolation(double* position,double* velocity){
    InitGeofenceServiceMsg(msg);
    memcpy(msg.name,"CheckFenceViolation",19);
    serializeRealArray(3,position,msg.buffer);
    SendSBMsg(msg);
    return msg.id;
}

int ServiceFence_GetWPFeasibility(double* positionA,double* positionB){
    InitGeofenceServiceMsg(msg);
    memcpy(msg.name,"CheckDirectPathFeasibility",26);
    serializeRealArray(3,positionA,msg.buffer);
    serializeRealArray(3,positionB,msg.buffer);
    SendSBMsg(msg);
    return msg.id;
}

int ServiceFence_GetRecoveryPosition(double* position){
    InitGeofenceServiceMsg(msg);
    memcpy(msg.name,"CheckDirectPathFeasibility",26);
    serializeRealArray(3,position,msg.buffer);
    SendSBMsg(msg);
    return msg.id;
}

int ServiceTraffic_GetTrafficConflict(bool bands,double* position,double* velocity){
    InitTrafficServiceMsg(msg);
    memcpy(msg.name,"MonitorTraffic",14);
    serializeBool(false,bands,msg.buffer);
    serializeRealArray(3,position,msg.buffer);
    serializeRealArray(3,velocity,msg.buffer);
    SendSBMsg(msg);
    return msg.id;
}

int ServiceTraffic_GetSafe2Turn(double* position,double* velocity,double headingA,double headingB){
    InitTrafficServiceMsg(msg);
    memcpy(msg.name,"CheckSafeToTurn",15);
    serializeRealArray(3,position,msg.buffer);
    serializeRealArray(3,velocity,msg.buffer);
    serializeReal(false,headingA,msg.buffer);
    serializeReal(false,headingB,msg.buffer);
    SendSBMsg(msg);
    return msg.id;
}

int ServiceTrajectory_GetWaypoint(char* planID,int index){
    InitTrajectoryServiceMsg(msg);
    memcpy(msg.name,"GetWaypoint",11);
    serializeString(5,planID,msg.buffer);
    serializeInt(false,index,msg.buffer);
    SendSBMsg(msg);
    return msg.id;
}

int ServiceTrajectory_GetNewPath(char* planID,char* algorithm,double* positionA,double* velocity,double* positionB){
    InitTrajectoryServiceMsg(msg);
    memcpy(msg.name,"FindNewPath",11);
    serializeString(5,planID,msg.buffer);
    if (!strcmp(algorithm, "GRID")) {
        serializeString(4,"GRID",msg.buffer);
    }else if (!strcmp(algorithm, "ASTAR")) {
        serializeString(5,"ASTAR",msg.buffer);
    }else if (!strcmp(algorithm, "RRT")) {
        serializeString(3,"RRT",msg.buffer);
    }
    serializeRealArray(3,positionA,msg.buffer);
    serializeRealArray(3,velocity,msg.buffer);
    serializeRealArray(3,positionB,msg.buffer);
    SendSBMsg(msg);
    return msg.id;
}

int ServiceTrajectory_GetXTrackDeviation(char* planID,int leg,double* position){
    InitTrajectoryServiceMsg(msg);
    memcpy(msg.name,"ComputeCrossTrackDeviation",26);
    serializeString(5,planID,msg.buffer);
    serializeInt(false,leg,msg.buffer);
    serializeRealArray(3,position,msg.buffer);
    SendSBMsg(msg);
    return msg.id;
}

int ServiceTrajectory_GetDistance(double* positionA,double* positionB){
    InitTrajectoryServiceMsg(msg);
    memcpy(msg.name,"ComputeDistance",15);
    serializeRealArray(3,positionA,msg.buffer);
    serializeRealArray(3,positionB,msg.buffer);
    SendSBMsg(msg);
    return msg.id;
}

int ServiceTrajectory_GetExitPoint(char* planID,double* position,int nextWP){
    InitTrajectoryServiceMsg(msg);
    memcpy(msg.name,"GetExitPoint",12);
    serializeString(5,planID,msg.buffer);
    serializeRealArray(3,position,msg.buffer);
    serializeInt(false,nextWP,msg.buffer);
    SendSBMsg(msg);
    return msg.id;
}

int ServiceTrajectory_GetInterceptHeading(char* planID,int nextWP,double* position){
    InitTrajectoryServiceMsg(msg);
    memcpy(msg.name,"GetInterceptHeadingToPlan",25);
    serializeString(5,planID,msg.buffer);
    serializeInt(false,nextWP,msg.buffer);
    serializeRealArray(3,position,msg.buffer);
    SendSBMsg(msg);
    return msg.id;
}

int ServiceTrajectory_GetInterceptManeuver(char* planID,int nextWP,double* position){
    InitTrajectoryServiceMsg(msg);
    memcpy(msg.name,"ManeuverToIntercept",19);
    serializeString(5,planID,msg.buffer);
    serializeInt(false,nextWP,msg.buffer);
    serializeRealArray(3,position,msg.buffer);
    SendSBMsg(msg);
    return msg.id;
}

void ServiceFence_DecodeFenceViolation(service_t* msg,bool* output){
    const char* b  = msg->buffer;
    b = deSerializeBoolArray(output,b);
}

void ServiceFence_DecodeWPFeasibility(service_t* msg,bool* output){
    const char* b = msg->buffer;
    b = deSerializeBool(false,output,b);
}

void ServiceFence_DecodeRecoveryPosition(service_t* msg,double* output){
    const char* b = msg->buffer;
    b = deSerializeRealArray(output,b);
}

void ServiceTraffic_DecodeTrafficConflict(service_t* msg,double* output){
    const char* b = msg->buffer;
    double resolution[4];
    b = deSerializeRealArray(resolution,b);
    *output = resolution[0];
}

void ServiceTraffic_DecodeSafe2Turn(service_t* msg,bool* output){
    const char* b = msg->buffer;
    b = deSerializeBool(false,output,b);
}

void ServiceTrajectory_DecodeWaypoint(service_t* msg,double* output){
    const char* b = msg->buffer;
    b = deSerializeRealArray(output,b);
}

void ServiceTrajectory_DecodeTotalWaypoints(service_t* msg,int* output){
    const char* b = msg->buffer;
    b = deSerializeInt(false,output,b);
}

void ServiceTrajectory_DecodeNewPath(service_t* msg,int* output){
    const char* b = msg->buffer;
    b = deSerializeInt(false,output,b);
}

void ServiceTrajectory_DecodeXTrackDeviation(service_t* msg,double* output){
    const char* b = msg->buffer;
    b = deSerializeReal(false,output,b);
}

void ServiceTrajectory_DecodeDistance(service_t* msg,double* output){
    const char* b = msg->buffer;
    b = deSerializeReal(false,output,b);
}

void ServiceTrajectory_DecodeExitPoint(service_t* msg,double* output){
    const char* b = msg->buffer;
    b = deSerializeRealArray(output,b);
}
void ServiceTrajectory_DecodeInterceptHeading(service_t* msg,double* output){
    const char* b = msg->buffer;
    b = deSerializeReal(false,output,b);
}
