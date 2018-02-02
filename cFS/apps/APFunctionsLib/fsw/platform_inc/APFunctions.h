//
// Created by Swee Balachandran on 11/28/17.
//

#ifndef APFUNCTIONS_H
#define APFUNCTIONS_H

#include "cfe.h"
#include "msgids/msgids.h"
#include "Icarous_msg.h"

#define InitMsg(cmd)\
ArgsCmd_t cmd; \
CFE_SB_InitMsg(&cmd,AP_COMMANDS_MID,sizeof(ArgsCmd_t),TRUE);


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


#endif //APFUNCTIONS_H
