//
// Created by Swee balachandran on 12/27/17.
//
#define EXTERN extern

#include <Plexil_msg.h>
#include "interface.h"

int8_t HandePlexilMessage(){

    plexil_interface_t *msg;
    msg = (plexil_interface_t *) appdataInt.INTERFACEMsgPtr;
    mavlink_message_t msgMavlink;
    control_mode_t mode;
    switch (msg->plxMsg.mType) {
        case _LOOKUP_:
            if(strcmp(msg->plxMsg.name,"missionStart") == 0){
                bool start = (int)startMission.param1?true:false;
                strcpy(plexilInput.plxMsg.name,msg->plxMsg.name);
                plexilInput.plxMsg.mType = _LOOKUP_RETURN_;
                plexilInput.plxMsg.rType = _BOOLEAN_;
                plexilInput.plxMsg.argsB[0] = start;
                SendSBMsg(plexilInput);
                startMission.param1 = 0;
            }
            break;

        case _COMMAND_:
            if(strcmp(msg->plxMsg.name,"ARM") == 0){
                mavlink_msg_command_long_pack(255,0,&msgMavlink,1,0,MAV_CMD_COMPONENT_ARM_DISARM,0,msg->plxMsg.argsI[0],0,0,0,0,0,0);
                break;
            }
            else if(strcmp(msg->plxMsg.name,"TAKEOFF") == 0){
                mavlink_msg_command_long_pack(255,0,&msgMavlink,1,0,MAV_CMD_NAV_TAKEOFF,0,1,0,0,0,0,0,0);
                break;
            }
            else if(strcmp(msg->plxMsg.name,"SETMODE")){
                if(msg->plxMsg.argsI[0] == _PASSIVE_){
                    mode = AUTO;
                }else if(msg->plxMsg.argsI[0] == _ACTIVE_){
                    mode = GUIDED;
                }
                mavlink_msg_set_mode_pack(255,0,&msgMavlink,0,1,mode);
                break;
            }
            else if(strcmp(msg->plxMsg.name,"LAND")){
                mavlink_msg_command_long_pack(255,0,&msgMavlink,1,0,MAV_CMD_NAV_LAND,0,1,0,0,0,0,0,0);
                break;
            }
            else if(strcmp(msg->plxMsg.name,"GOTOWP")){
                uint16_t tempSeq = (int)msg->plxMsg.argsI[0];
                uint16_t seq = -1;
                for(int i=0;i<=tempSeq;i++){
                    seq++;
                    int val = (appdataInt.waypoint_type[seq] == MAV_CMD_NAV_WAYPOINT) ||
                              (appdataInt.waypoint_type[seq] == MAV_CMD_NAV_SPLINE_WAYPOINT);
                    if (!val){
                        i = i-1 ;
                    }
                }
                mavlink_msg_mission_set_current_pack(255,0,&msgMavlink,1,0,seq);
                break;
            }
            else if(strcmp(msg->plxMsg.name,"SETPOS")){
                int64 latitude  = msg->plxMsg.argsD[0];
                int64 longitude = msg->plxMsg.argsD[1];
                int64 altitude  = msg->plxMsg.argsD[2];
                mavlink_msg_set_position_target_global_int_pack(255,0,&msgMavlink,0,1,0,MAV_FRAME_GLOBAL_RELATIVE_ALT_INT,
                                                                0b0000111111111000,(int)(latitude*1E7),(int)(longitude*1E7),(altitude),
                                                                0,0,0,0,0,0,0,0);
                break;
            }
            else if(strcmp(msg->plxMsg.name,"SETVEL")){
                double vx = msg->plxMsg.argsD[0];
                double vy = msg->plxMsg.argsD[1];
                double vz = msg->plxMsg.argsD[2];
                mavlink_msg_set_position_target_local_ned_pack(255,0,&msgMavlink,0,1,0,MAV_FRAME_LOCAL_NED, 0b0000111111000111,0,0,0,
                                                               (float)vx,(float)vy,(float)vz,
                                                               0,0,0,0,0);
                break;
            }
            else if(strcmp(msg->plxMsg.name,"SETYAW")){
                double heading = msg->plxMsg.argsD[0];
                int relative = msg->plxMsg.argsI[0];
                mavlink_msg_command_long_pack(255,0,&msg,1,0,MAV_CMD_CONDITION_YAW,0,
                                              (float)heading,0,1,(float)relative,0,0,0);
                break;
            }
            else if(strcmp(msg->plxMsg.name,"SETSPEED")){
                double speed = msg->plxMsg.argsD[0];
                mavlink_msg_command_long_pack(255,0,&msg,1,0,MAV_CMD_DO_CHANGE_SPEED,0,
                                              1,(float)speed,0,0,0,0,0);
                break;
            }
            break;
    }

}