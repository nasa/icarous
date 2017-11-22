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
                    c_Reset();
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

    return 0;
}


uint8_t ProcessPlexilLookup(PlexilCommandMsg* Msg){
    return 0;
}

