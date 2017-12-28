//
// Created by Swee balachandran on 12/27/17.
//
#define EXTERN extern

#include <Plexil_msg.h>
#include <Icarous_msg.h>
#include "interface.h"

bool HandlePlexilMessages(mavlink_message_t *msgMavlink){

    plexil_interface_t *msg;
    msg = (plexil_interface_t *) appdataInt.INTERFACEMsgPtr;
    control_mode_t mode;
    bool send = false;
    memset(&plexilInput.plxMsg, 0, sizeof(plexilInput.plxMsg));
    plexilInput.plxMsg.mType = _LOOKUP_RETURN_;
    strcpy(plexilInput.plxMsg.name,msg->plxMsg.name);
    switch (msg->plxMsg.mType) {
        case _LOOKUP_:
            if(!strcmp(msg->plxMsg.name,"missionStart")){
                bool start = (int)startMission.param1?true:false;
                plexilInput.plxMsg.rType = _BOOLEAN_;
                plexilInput.plxMsg.argsB[0] = start;
                SendSBMsg(plexilInput);
                startMission.param1 = 0;
            }
            else if(!strcmp(msg->plxMsg.name,"armStatus")){
                int8_t result = 0;
                OS_printf("arming status check\n");
                if (ack.name == _ARM_) {
                    result = (int) ack.result == 0 ? 1 : 0;
                    plexilInput.plxMsg.rType = _INTEGER_;
                    plexilInput.plxMsg.argsI[0] = result;
                    SendSBMsg(plexilInput);
                }
            }
            else if(!strcmp(msg->plxMsg.name,"takeoffStatus")){
                int8_t result = 0;
                if (ack.name == _TAKEOFF_) {
                    OS_printf("takeoff status check %d\n", ack.result);
                    result = (int) ack.result == 0 ? 1 : 0;
                    plexilInput.plxMsg.rType = _INTEGER_;
                    plexilInput.plxMsg.argsI[0] = result;
                    SendSBMsg(plexilInput);
                }
            }else if(1){

            }else{
                plexilInput.plxMsg.rType = _INTEGER_;
                plexilInput.plxMsg.argsI[0] = -1;
                SendSBMsg(plexilInput);
            }
            break;

        case _COMMAND_: {
            OS_printf("command\n");
            send = true;
            if (strcmp(msg->plxMsg.name, "ArmMotors") == 0) {
                OS_printf("Arming\n");
                mavlink_msg_command_long_pack(255, 0, msgMavlink, 1, 0, MAV_CMD_COMPONENT_ARM_DISARM, 0,
                                              1, 0, 0, 0, 0, 0, 0);
                break;
            } else if (strcmp(msg->plxMsg.name, "Takeoff") == 0) {
                OS_printf("Takeoff off to:%f\n",msg->plxMsg.argsD[0]);
                mavlink_msg_command_long_pack(255, 0, msgMavlink, 1, 0, MAV_CMD_NAV_TAKEOFF, 0, 0, 0, 0, 0, 0, 0, (float)msg->plxMsg.argsD[0]);
                break;
            } else if (strcmp(msg->plxMsg.name, "SetMode") == 0) {
                OS_printf("Setting mode\n");
                if (!strcmp(msg->plxMsg.string, "PASSIVE")) {
                    mode = AUTO;
                } else if (!strcmp(msg->plxMsg.string, "ACTIVE")) {
                    mode = GUIDED;
                }
                mavlink_msg_set_mode_pack(255, 0, msgMavlink, 0, 1, mode);
                break;
            } else if (strcmp(msg->plxMsg.name, "Land") == 0) {
                mavlink_msg_command_long_pack(255, 0, msgMavlink, 1, 0, MAV_CMD_NAV_LAND, 0, 0, 0, 0, 0, 0, 0, 0);
                break;
            } else if (strcmp(msg->plxMsg.name, "GOTOWP") == 0) {
                uint16_t tempSeq = (int) msg->plxMsg.argsI[0];
                uint16_t seq = -1;
                for (int i = 0; i <= tempSeq; i++) {
                    seq++;
                    int val = (appdataInt.waypoint_type[seq] == MAV_CMD_NAV_WAYPOINT) ||
                              (appdataInt.waypoint_type[seq] == MAV_CMD_NAV_SPLINE_WAYPOINT);
                    if (!val) {
                        i = i - 1;
                    }
                }
                mavlink_msg_mission_set_current_pack(255, 0, msgMavlink, 1, 0, seq);
                break;
            } else if (strcmp(msg->plxMsg.name, "SETPOS") == 0) {
                int64 latitude = msg->plxMsg.argsD[0];
                int64 longitude = msg->plxMsg.argsD[1];
                int64 altitude = msg->plxMsg.argsD[2];
                mavlink_msg_set_position_target_global_int_pack(255, 0, msgMavlink, 0, 1, 0,
                                                                MAV_FRAME_GLOBAL_RELATIVE_ALT_INT,
                                                                0b0000111111111000, (int) (latitude * 1E7),
                                                                (int) (longitude * 1E7), (altitude),
                                                                0, 0, 0, 0, 0, 0, 0, 0);
                break;
            } else if (strcmp(msg->plxMsg.name, "SETVEL") == 0) {
                double vx = msg->plxMsg.argsD[0];
                double vy = msg->plxMsg.argsD[1];
                double vz = msg->plxMsg.argsD[2];
                mavlink_msg_set_position_target_local_ned_pack(255, 0, msgMavlink, 0, 1, 0, MAV_FRAME_LOCAL_NED,
                                                               0b0000111111000111, 0, 0, 0,
                                                               (float) vx, (float) vy, (float) vz,
                                                               0, 0, 0, 0, 0);
                break;
            } else if (strcmp(msg->plxMsg.name, "SETYAW") == 0) {
                double heading = msg->plxMsg.argsD[0];
                int relative = msg->plxMsg.argsI[0];
                mavlink_msg_command_long_pack(255, 0, msg, 1, 0, MAV_CMD_CONDITION_YAW, 0,
                                              (float) heading, 0, 1, (float) relative, 0, 0, 0);
                break;
            } else if (strcmp(msg->plxMsg.name, "SETSPEED") == 0) {
                double speed = msg->plxMsg.argsD[0];
                mavlink_msg_command_long_pack(255, 0, msg, 1, 0, MAV_CMD_DO_CHANGE_SPEED, 0,
                                              1, (float) speed, 0, 0, 0, 0, 0);
                break;
            }
            break;
        }
    }

    return send;
}