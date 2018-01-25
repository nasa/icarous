//
// Created by Swee Balachandran on 11/28/17.
//

#include "APFunctions.h"

int32 APFunctionsLibInit(void)
{

    OS_printf ("APFunctions Lib Initialized.\n");

    return CFE_SUCCESS;

}

// Set Mode
void SetMode(icarous_control_mode_t mode){
    InitMsg(cmd);
    cmd.name = _SETMODE_;
    cmd.param1 = mode;
    SendSBMsg(cmd);
}

// Arm Motors
void ArmThrottles(uint8_t c){
    InitMsg(cmd);
    cmd.name = _ARM_;
    cmd.param1 = c;
    SendSBMsg(cmd);
}

// Set Next WP
void SetNextMissionItem(int nextWP){
    InitMsg(cmd);
    cmd.name = _GOTOWP_;
    cmd.param1 = nextWP;
    SendSBMsg(cmd);
}

// Takeoff
void Takeoff(float alt){
    InitMsg(cmd);
    cmd.name = _TAKEOFF_;
    cmd.param1 = alt;
    SendSBMsg(cmd);
}

// Land
void Land(){
    InitMsg(cmd);
    cmd.name = _LAND_;
    cmd.param5 = 0;
    cmd.param6 = 0;
    cmd.param7 = 0;
    SendSBMsg(cmd);
}

// Set yaw
void SetYaw(uint8_t relative,double heading){
    InitMsg(cmd);
    cmd.name = _SETYAW_;
    cmd.param1 = heading;
    cmd.param2 = 0;
    cmd.param3 = 1;
    cmd.param4 = relative;
    SendSBMsg(cmd);
}

// Set GPS Position
void SetGPSPosition(double lat,double lon,double alt){
    InitMsg(cmd);
    cmd.name = _SETPOS_;
    cmd.param1 = lat;
    cmd.param2 = lon;
    cmd.param3 = alt;
    SendSBMsg(cmd);
}

// Set Velocity
void SetVelocity(double Vn, double Ve, double Vu){
    InitMsg(cmd);
    cmd.name = _SETVEL_;
    cmd.param1 = Vn;
    cmd.param2 = Ve;
    cmd.param3 = Vu;
    SendSBMsg(cmd);
}

// Set Speed
void SetSpeed(float speed){
    InitMsg(cmd);
    cmd.name = _SETSPEED_;
    cmd.param1 = speed;
    SendSBMsg(cmd);
}