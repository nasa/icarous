//
// Created by Swee Balachandran on 11/21/17.
//
#define EXTERN extern
#include "plexil.h"
#include "msgids/msgids.h"
#include "APFunctions.h"
#include "stdbool.h"

void PLEXIL_DAQ(void){
    OS_TaskRegister();
    uint32 RunStatus = CFE_ES_APP_RUN;
    int32 status;

    while(plexilAppData.threadState == 1) {
        status = CFE_SB_RcvMsg(&plexilAppData.FlightData_MsgPtr, plexilAppData.FlightData_Pipe, CFE_SB_PEND_FOREVER);
        if (status == CFE_SUCCESS) {

            CFE_SB_MsgId_t MsgId;
            MsgId = CFE_SB_GetMsgId(plexilAppData.FlightData_MsgPtr);

            switch (MsgId) {
                case ICAROUS_WP_MID: {
                    waypoint_t *msg = (waypoint_t *) plexilAppData.FlightData_MsgPtr;
                    c_AddMissionItem(plexilAppData.fData, msg);
                    //OS_printf("Added mission item\n");
                    break;
                }

                case ICAROUS_WPREACHED_MID: {
                    //TODO: check if this needs to be implemented here
                    break;
                }

                case ICAROUS_STARTMISSION_MID: {
                    ArgsCmd_t *cmd;
                    cmd = (ArgsCmd_t *) plexilAppData.FlightData_MsgPtr;
                    c_SetStartMissionFlag(plexilAppData.fData, (uint8_t) cmd->param1);
                    break;
                }

                case ICAROUS_RESET_MID: {

                    NoArgsCmd_t *cmd;
                    cmd = (NoArgsCmd_t *)  plexilAppData.FlightData_MsgPtr;
                    c_Reset(plexilAppData.fData);
                    break;
                }

                case ICAROUS_POSITION_MID: {
                    position_t *pos;
                    pos = (position_t *) plexilAppData.FlightData_MsgPtr;
                    c_InputState(plexilAppData.fData,pos->time_gps,
                                 pos->latitude, pos->longitude, pos->altitude_rel,
                                 pos->vx, pos->vy, pos->vz);
                    break;
                }
            }
        }
    }
}

uint8_t ProcessPlexilCommand(PlexilCommandMsg* Msg){

    if(strcmp(Msg->name,"RunIdleChecks") == 0){
        uint8_t rval = runIdleChecks();
        PlexilCommandMsg returnMsg;
        returnMsg.id    = Msg->id;
        returnMsg.mType = _COMMAND_RETURN_;
        returnMsg.rType = _INTEGER_;
        returnMsg.argsI[0] = rval;
        plexil_return(plexilAppData.adap,&returnMsg);
        return 1;
    }else if(strcmp(Msg->name,"ThrottleUp") == 0){
        OS_printf("Throttle Up\n");
        uint8_t rval = ThrottleUp();
        PlexilCommandMsg returnMsg;
        returnMsg.id    = Msg->id;
        returnMsg.mType = _COMMAND_RETURN_;
        returnMsg.rType = _BOOLEAN_;
        returnMsg.argsB[0] = (bool)rval;
        plexil_return(plexilAppData.adap,&returnMsg);
        return 1;
    }else if(strcmp(Msg->name,"SetMode") == 0){
        OS_printf("Set Mode\n");
        char modeName[50];
        memcpy(modeName,Msg->string,sizeof(Msg->string));
        if (strcmp(modeName,"ACTIVE") == 0){
            SetMode(_ACTIVE_);
        }else{
            SetMode(_PASSIVE_);
        }
        return 1;
    }else if(strcmp(Msg->name,"ArmMotors") == 0){
        OS_printf("Arming motors\n");
        ArmThrottles(1);
        return 1;
    }else if(strcmp(Msg->name,"SetNextWPParameters") == 0){
        OS_printf("Setting Next WP\n");
        c_InputNextMissionWP(plexilAppData.fData,Msg->argsI[0]);
        SetNextMissionItem(Msg->argsI[0]);
        return 1;
    }else if(strcmp(Msg->name,"StartLandingSequence") == 0){
        OS_printf("Landing\n");
        Land();
        return 1;
    }else if(strcmp(Msg->name,"SetYaw") == 0){
        OS_printf("Yawing\n");
        SetYaw(0,Msg->argsD[0]);
        return 1;
    }

    return 0;
}


uint8_t ProcessPlexilLookup(PlexilCommandMsg* Msg){

    if(strcmp(Msg->name,"altitudeAGL") == 0){
        PlexilCommandMsg returnMsg;
        returnMsg.id = Msg->id;
        returnMsg.mType = _LOOKUP_RETURN_;
        returnMsg.rType = _REAL_;
        returnMsg.argsD[0] = c_GetAltitude(plexilAppData.fData);
        plexil_return(plexilAppData.adap,&returnMsg);
    }else if(strcmp(Msg->name,"totalWP") == 0){
        PlexilCommandMsg returnMsg;
        returnMsg.id = Msg->id;
        returnMsg.mType = _LOOKUP_RETURN_;
        returnMsg.rType = _INTEGER_;
        returnMsg.argsI[0] = c_GetTotalMissionWP(plexilAppData.fData);
        plexil_return(plexilAppData.adap,&returnMsg);
    }


    return 0;
}

//Idle Check commands
uint8_t runIdleChecks(){
    int start  = c_GetStartMissionFlag(plexilAppData.fData);
    int fpsize = c_GetMissionPlanSize(plexilAppData.fData);

    if( start == 0 && start < fpsize){
            c_ConstructMissionPlan(plexilAppData.fData);
            c_InputNextMissionWP(plexilAppData.fData,0);
            //SendStatusText("Starting mission",19);
            OS_printf("starting mission\n");
            return 1;
    }
    else if(start > 0 && start < fpsize ){
        c_ConstructMissionPlan(plexilAppData.fData);
        c_InputNextMissionWP(plexilAppData.fData,start);

        //SendStatusText("Flying to waypoint",19);
        return 1;
    }else{
        //SendStatusText("No flightplan uploaded",23);
        return 0;
    }
}

uint8_t ThrottleUp(){
    double targetAlt = c_GetTakeoffAlt(plexilAppData.fData);
    Takeoff(targetAlt);
    uint8_t rval;
    if(c_CheckAck(plexilAppData.fData,_TAKEOFF_)){
        c_InputNextMissionWP(plexilAppData.fData,1);
        return 1;
    }else{
        return 0;
    }
}
