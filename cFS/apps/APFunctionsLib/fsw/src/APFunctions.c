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
    msg.sType = _command_;
    memcpy(msg.name,"CheckFenceViolation",19);
    char* b  = msg.buffer;
    b = serializeRealArray(3,position,b);
    b = serializeRealArray(3,velocity,b);
    SendSBMsg(msg);
    return msg.id;
}

int ServiceFence_GetWPFeasibility(double* positionA,double* positionB){
    InitGeofenceServiceMsg(msg);
    msg.sType = _command_;
    memcpy(msg.name,"CheckDirectPathFeasibility",26);
    char* b  = msg.buffer;
    b = serializeRealArray(3,positionA,b);
    b = serializeRealArray(3,positionB,b);
    SendSBMsg(msg);
    return msg.id;
}

int ServiceFence_GetRecoveryPosition(double* position){
    InitGeofenceServiceMsg(msg);
    msg.sType = _command_;
    memcpy(msg.name,"GetRecoveryPosition",19);
    serializeRealArray(3,position,msg.buffer);
    SendSBMsg(msg);
    return msg.id;
}

int ServiceTraffic_GetTrafficConflict(bool bands,double* position,double* velocity){
    InitTrafficServiceMsg(msg);
    msg.sType = _command_;
    memcpy(msg.name,"MonitorTraffic",14);
    char* b  = msg.buffer;
    b = serializeBool(false,bands,b);
    b = serializeRealArray(3,position,b);
    b = serializeRealArray(3,velocity,b);
    SendSBMsg(msg);
    return msg.id;
}

int ServiceTraffic_GetSafe2Turn(double* position,double* velocity,double headingA,double headingB){
    InitTrafficServiceMsg(msg);
    msg.sType = _command_;
    memcpy(msg.name,"CheckSafeToTurn",15);
    char* b  = msg.buffer;
    b = serializeRealArray(3,position,b);
    b = serializeRealArray(3,velocity,b);
    b = serializeReal(false,headingA,b);
    b = serializeReal(false,headingB,b);
    SendSBMsg(msg);
    return msg.id;
}

int ServiceTrajectory_GetWaypoint(char* planID,int index){
    InitTrajectoryServiceMsg(msg);
    msg.sType = _command_;
    memcpy(msg.name,"GetWaypoint",11);
    char* b  = msg.buffer;
    b = serializeString(5,planID,b);
    b = serializeInt(false,index,b);
    SendSBMsg(msg);
    return msg.id;
}

int ServiceTrajectory_GetTotalWaypoints(char* planID){
    InitTrajectoryServiceMsg(msg);
    msg.sType = _command_;
    memcpy(msg.name,"GetTotalWaypoints",17);
    char* b  = msg.buffer;
    b = serializeString(5,planID,b);
    SendSBMsg(msg);
    return msg.id;
}

int ServiceTrajectory_GetNewPath(char* planID,char* algorithm,double* positionA,double* velocity,double* positionB){
    InitTrajectoryServiceMsg(msg);
    msg.sType = _command_;
    memcpy(msg.name,"FindNewPath",11);
    char* b  = msg.buffer;
    b = serializeString(5,planID,b);
    if (!strcmp(algorithm, "GRID")) {
        b = serializeString(4,"GRID",b);
    }else if (!strcmp(algorithm, "ASTAR")) {
        b = serializeString(5,"ASTAR",b);
    }else if (!strcmp(algorithm, "RRT")) {
        b = serializeString(3,"RRT",b);
    }else{
        OS_printf("unknown algorithm\n");
    }
    b = serializeRealArray(3,positionA,b);
    b = serializeRealArray(3,velocity,b);
    b = serializeRealArray(3,positionB,b);
    SendSBMsg(msg);
    return msg.id;
}

int ServiceTrajectory_GetXTrackDeviation(char* planID,int leg,double* position){
    InitTrajectoryServiceMsg(msg);
    msg.sType = _command_;
    memcpy(msg.name,"ComputeCrossTrackDeviation",26);
    char* b  = msg.buffer;
    b = serializeString(5,planID,b);
    b = serializeInt(false,leg,b);
    b = serializeRealArray(3,position,b);
    SendSBMsg(msg);
    return msg.id;
}

int ServiceTrajectory_GetDistance(double* positionA,double* positionB){
    InitTrajectoryServiceMsg(msg);
    msg.sType = _command_;
    memcpy(msg.name,"ComputeDistance",15);
    char* b  = msg.buffer;
    b = serializeRealArray(3,positionA,b);
    b = serializeRealArray(3,positionB,b);
    SendSBMsg(msg);
    return msg.id;
}

int ServiceTrajectory_GetExitPoint(char* planID,double* position,int nextWP){
    InitTrajectoryServiceMsg(msg);
    msg.sType = _command_;
    memcpy(msg.name,"GetExitPoint",12);
    char* b  = msg.buffer;
    b = serializeString(5,planID,b);
    b = serializeRealArray(3,position,b);
    b = serializeInt(false,nextWP,b);
    SendSBMsg(msg);
    return msg.id;
}

int ServiceTrajectory_GetInterceptHeading(char* planID,int nextWP,double* position){
    InitTrajectoryServiceMsg(msg);
    msg.sType = _command_;
    memcpy(msg.name,"GetInterceptHeadingToPlan",25);
    char* b  = msg.buffer;
    b = serializeString(5,planID,b);
    b = serializeInt(false,nextWP,b);
    b = serializeRealArray(3,position,b);
    SendSBMsg(msg);
    return msg.id;
}

int ServiceTrajectory_GetInterceptManeuver(char* planID,int nextWP,double* position){
    InitTrajectoryServiceMsg(msg);
    msg.sType = _command_;
    memcpy(msg.name,"ManeuverToIntercept",19);
    char* b  = msg.buffer;
    b = serializeString(5,planID,b);
    b = serializeInt(false,nextWP,b);
    b = serializeRealArray(3,position,b);
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

void ServiceTraffic_DecodeTrafficConflict(service_t* msg,bool* conflict,double* output){
    const char* b = msg->buffer;
    double resolution[4];
    b = deSerializeRealArray(resolution,b);
    *conflict = resolution[0];
    output[0] = resolution[1];
    output[1] = resolution[2];
    output[2] = resolution[3];
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

void ServiceTrajectory_DecodeInterceptManeuver(service_t* msg,double* output){
    const char* b = msg->buffer;
    b = deSerializeRealArray(output,b);
}
