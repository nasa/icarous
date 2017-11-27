//
// Created by Swee Balachandran on 11/21/17.
//
#define EXTERN extern
#include "plexil.h"
#include "msgids/msgids.h"

void PLEXIL_DAQ(void){

    uint32 RunStatus = CFE_ES_APP_RUN;
    int32 status;
    while(CFE_ES_RunLoop(&RunStatus) == TRUE) {
        status = CFE_SB_RcvMsg(&plexilAppData.FlightData_MsgPtr, plexilAppData.FlightData_Pipe, CFE_SB_PEND_FOREVER);

        if (status == CFE_SUCCESS) {

            CFE_SB_MsgId_t MsgId;
            MsgId = CFE_SB_GetMsgId(plexilAppData.FlightData_MsgPtr);

            switch (MsgId) {
                case ICAROUS_WP_MID: {
                    waypoint_t *msg = (waypoint_t *) plexilAppData.FlightData_MsgPtr;
                    c_addMissionItem(plexilAppData.fData, msg);
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

    OS_printf("command name %s\n",Msg->name);
    if(strcmp(Msg->name,"RunIdleChecks") == 0){
        uint8_t rval = runIdleChecks();
        PlexilCommandMsg returnMsg;
        returnMsg.id    = Msg->id;
        returnMsg.mType = _COMMAND_RETURN_;
        returnMsg.rType = _INTEGER_;
        returnMsg.argsI[0] = rval;
        plexil_return(plexilAppData.adap,&returnMsg);
        OS_printf("returning rval of %d to plexil \n",rval);
        return 1;
    }else if(strcmp(Msg->name,"ThrottleUp") == 0){

    }else if(strcmp(Msg->name,"SetMode") == 0){

    }else if(strcmp(Msg->name,"ArmMotors") == 0){

    }else if(strcmp(Msg->name,"SetNextWPParameters") == 0){

    }else if(strcmp(Msg->name,"StartLandingSequence") == 0){

    }else if(strcmp(Msg->name,"SetYaw") == 0){

    }

    return 0;
}


uint8_t ProcessPlexilLookup(PlexilCommandMsg* Msg){
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


// Throttle up


// Set Mode


// Arm Motors


// Set Next WP


// Land


// Set Yaw

