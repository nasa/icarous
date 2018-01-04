//
// Created by Swee balachandran on 12/27/17.
//
#define EXTERN extern

#include <Plexil_msg.h>
#include <Icarous_msg.h>
#include "interface.h"

bool IntfPlxMsgHandler(mavlink_message_t *msgMavlink){

    plexil_interface_t *msg;
    msg = (plexil_interface_t *) appdataInt.INTERFACEMsgPtr;
    control_mode_t mode;
    bool send = false;

    // Initialize plexilInput message
    memset(&plexilInput.plxData, 0, sizeof(plexilInput.plxData));
    CFE_SB_InitMsg(&plexilInput,PLEXIL_INPUT_MID,sizeof(plexil_interface_t),TRUE);
    plexilInput.plxData.mType = _LOOKUP_RETURN_;
    strcpy(plexilInput.plxData.name,msg->plxData.name);
    char* b= plexilInput.plxData.buffer;

    switch (msg->plxData.mType) {
        case _LOOKUP_: {

            if (CHECK_NAME(msg->plxData, "missionStart")) {
                bool start = (int) startMission.param1 ? true : false;
                startMission.param1 = 0;
                b = serializeBool(false, start, b);
                SendSBMsg(plexilInput);
            } else if (CHECK_NAME(msg->plxData, "armStatus")) {
                int32_t result = -1;
                OS_printf("arming status check\n");
                if (ack.name == _ARM_) {
                    result = (int) ack.result == 0 ? 1 : 0;
                }
                b = serializeInt(false, result, b);
                SendSBMsg(plexilInput);
            } else if (CHECK_NAME(msg->plxData, "takeoffStatus")) {
                int32_t result = -1;
                if (ack.name == _TAKEOFF_) {
                    OS_printf("takeoff status check %d\n", ack.result);
                    result = (int) ack.result == 0 ? 1 : 0;
                }
                b = serializeInt(false, result, b);
                SendSBMsg(plexilInput);
            } else if (CHECK_NAME(msg->plxData, "position")) {
                double _position[3] = {position.latitude, position.longitude, position.altitude_rel};
                b = serializeRealArray(3, _position, b);
                SendSBMsg(plexilInput);
            } else if (CHECK_NAME(msg->plxData, "velocity")) {
                double angle = 360 + atan2(position.vy, position.vx) * 180 / M_PI;
                double track = fmod(angle, 360);
                double groundSpeed = sqrt(pow(position.vx, 2) + pow(position.vy, 2));
                double verticalSpeed = position.vz;
                double _velocity[3] = {track, groundSpeed, verticalSpeed};
                b = serializeRealArray(3, _velocity, b);
                SendSBMsg(plexilInput);
            } else if (CHECK_NAME(msg->plxData, "numMissionWP")) {
                //TODO: move this into the trajectory app?
                int32_t result = appdataInt.numWaypoints;
                b = serializeInt(false, result, b);
                SendSBMsg(plexilInput);
            } else if(CHECK_NAME(msg->plxData, "nextMissionWPIndex")){
                int32_t result = appdataInt.nextWaypointIndex;
                b = serializeInt(false,result,b);
                SendSBMsg(plexilInput);
                //OS_printf("Next mission wp index:%d\n",result);
            } else {
                //OS_printf("******* unhandled lookup ************\n");
                //int32_t result = -1;
                //b = serializeInt(false,result,b);
            }
            break;
        }

        case _COMMAND_: {
            send = true;
            b = msg->plxData.buffer;
            if (strcmp(msg->plxData.name, "ArmMotors") == 0) {
                OS_printf("Arming\n");
                mavlink_msg_command_long_pack(255, 0, msgMavlink, 1, 0, MAV_CMD_COMPONENT_ARM_DISARM, 0,
                                              1, 0, 0, 0, 0, 0, 0);
                break;
            } else if (strcmp(msg->plxData.name, "Takeoff") == 0) {
                double takeoffAlt;
                b = deSerializeReal(false,&takeoffAlt,b);
                OS_printf("Takeoff off to:%f\n",takeoffAlt);
                mavlink_msg_command_long_pack(255, 0, msgMavlink, 1, 0, MAV_CMD_NAV_TAKEOFF, 0, 0, 0, 0, 0, 0, 0, (float)takeoffAlt);
                break;
            } else if (strcmp(msg->plxData.name, "SetMode") == 0) {
                OS_printf("Setting mode\n");
                char modeName[15];
                memset(modeName,0,15);
                b = deSerializeString(modeName,b);
                if (!strcmp(modeName, "PASSIVE")) {
                    mode = AUTO;
                } else if (!strcmp(modeName, "ACTIVE")) {
                    mode = GUIDED;
                }
                mavlink_msg_set_mode_pack(255, 0, msgMavlink, 0, 1, mode);
                break;
            } else if (strcmp(msg->plxData.name, "Land") == 0) {
                mavlink_msg_command_long_pack(255, 0, msgMavlink, 1, 0, MAV_CMD_NAV_LAND, 0, 0, 0, 0, 0, 0, 0, 0);
                break;
            } else if (strcmp(msg->plxData.name, "SetNextMissionWP") == 0) {
                uint32_t tempSeq;
                b = deSerializeInt(false,&tempSeq,b);
                uint32_t seq = -1;
                appdataInt.nextWaypointIndex = tempSeq;
                for (int i = 0; i <= tempSeq; i++) {
                    seq++;
                    int val = (appdataInt.waypoint_type[seq] == MAV_CMD_NAV_WAYPOINT) ||
                              (appdataInt.waypoint_type[seq] == MAV_CMD_NAV_SPLINE_WAYPOINT);
                    if (!val) {
                        i = i - 1;
                    }
                }
                mavlink_msg_mission_set_current_pack(255, 0, msgMavlink, 1, 0, seq);
                OS_printf("Setting next mission WP %d\n",seq);
                break;
            } else if (strcmp(msg->plxData.name, "SetPos") == 0) {
                double _position[3];
                b = deSerializeRealArray(_position,b);
                double latitude = _position[0];
                double longitude =_position[1];
                double altitude = _position[2];
                mavlink_msg_set_position_target_global_int_pack(255, 0, msgMavlink, 0, 1, 0,
                                                                MAV_FRAME_GLOBAL_RELATIVE_ALT_INT,
                                                                0b0000111111111000, (int) (latitude * 1E7),
                                                                (int) (longitude * 1E7), (int) (altitude),
                                                                0, 0, 0, 0, 0, 0, 0, 0);
                //OS_printf("Setting position\n");
                break;
            } else if (strcmp(msg->plxData.name, "SETVEL") == 0) {

                double _velocity[3];
                b = deSerializeRealArray(_velocity,b);
                double vx = _velocity[0];
                double vy =_velocity[1];
                double vz = _velocity[2];

                mavlink_msg_set_position_target_local_ned_pack(255, 0, msgMavlink, 0, 1, 0, MAV_FRAME_LOCAL_NED,
                                                               0b0000111111000111, 0, 0, 0,
                                                               (float) vx, (float) vy, (float) vz,
                                                               0, 0, 0, 0, 0);
                break;
            } else if (strcmp(msg->plxData.name, "SETYAW") == 0) {
                double heading;
                uint32_t relative;
                b = deSerializeReal(false,&heading,b);
                b = deSerializeInt(false,&relative,b);
                mavlink_msg_command_long_pack(255, 0, msg, 1, 0, MAV_CMD_CONDITION_YAW, 0,
                                              (float) heading, 0, 1, (float) relative, 0, 0, 0);
                break;
            } else if (strcmp(msg->plxData.name, "SETSPEED") == 0) {
                double speed;
                deSerializeReal(false,&speed,b);
                mavlink_msg_command_long_pack(255, 0, msg, 1, 0, MAV_CMD_DO_CHANGE_SPEED, 0,
                                              1, (float) speed, 0, 0, 0, 0, 0);
                break;
            } else{
                send =false;
            }
            break;
        }
    }

    return send;
}