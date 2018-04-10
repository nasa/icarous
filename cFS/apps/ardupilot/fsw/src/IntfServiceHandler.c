//
// Created by Swee balachandran on 12/27/17.
//
#define EXTERN extern

#include <Icarous_msg.h>
#include "ardupilot.h"

bool IntfServiceHandler(mavlink_message_t *msgMavlink){

    service_t *msg;
    msg = (service_t *) appdataInt.INTERFACEMsgPtr;
    control_mode_t mode;
    bool send = false;

    // Initialize plexilInput message
    CFE_SB_InitMsg(&intfServiceResponse,SERVICE_INTERFACE_RESPONSE_MID,sizeof(service_t),TRUE);
    intfServiceResponse.id = msg->id;
    intfServiceResponse.sType = _lookup_return_;
    strcpy(intfServiceResponse.name,msg->name);

    switch (msg->sType) {
        case _lookup_: {
            char* b=intfServiceResponse.buffer;
            if (CHECKNAME((*msg), "missionStart")) {
                //OS_printf("received mission start\n");
                int32_t  start = (int) startMission.param1;
                startMission.param1 = -1;
                b = serializeInt(false, start, b);
                SendSBMsg(intfServiceResponse);
            } else if (CHECKNAME((*msg), "armStatus")) {
                int32_t result = -1;
                if (ack.name == _ARM_) {
                    result = (int) ack.result == 0 ? 1 : 0;
                }
                b = serializeInt(false, result, b);
                SendSBMsg(intfServiceResponse);
            } else if (CHECKNAME((*msg), "takeoffStatus")) {
                int32_t result = -1;
                if (ack.name == _TAKEOFF_) {
                    result = (int) ack.result == 0 ? 1 : 0;
                }
                b = serializeInt(false, result, b);
                SendSBMsg(intfServiceResponse);
            } else if (CHECKNAME((*msg), "position")) {
                double _position[3] = {position.latitude, position.longitude, position.altitude_rel};
                b = serializeRealArray(3, _position, b);
                SendSBMsg(intfServiceResponse);
            } else if (CHECKNAME((*msg), "velocity")) {
                double angle = 360 + atan2(position.vy, position.vx) * 180 / M_PI;
                double track = fmod(angle, 360);
                double groundSpeed = sqrt(pow(position.vx, 2) + pow(position.vy, 2));
                double verticalSpeed = position.vz;
                double _velocity[3] = {track, groundSpeed, verticalSpeed};
                b = serializeRealArray(3, _velocity, b);
                SendSBMsg(intfServiceResponse);
            } else if (CHECKNAME((*msg), "numMissionWP")) {
                //TODO: move this into the trajectory app?
                int32_t result = appdataInt.numWaypoints;
                b = serializeInt(false, result, b);
                SendSBMsg(intfServiceResponse);
            } else if(CHECKNAME((*msg), "nextMissionWPIndex")){
                int32_t result = appdataInt.nextWaypointIndex;
                b = serializeInt(false,result,b);
                SendSBMsg(intfServiceResponse);
                //OS_printf("Next mission wp index:%d\n",result);
            } else {
                //OS_printf("******* unhandled lookup ************\n");
                //int32_t result = -1;
                //b = serializeInt(false,result,b);
            }
            break;
        }

        case _command_: {
            send = true;
            const char* b = msg->buffer;
            if (CHECKNAME((*msg), "ArmMotors")) {
                //OS_printf("Arming\n");
                mavlink_msg_command_long_pack(255, 0, msgMavlink, 1, 0, MAV_CMD_COMPONENT_ARM_DISARM, 0,
                                              1, 0, 0, 0, 0, 0, 0);
                break;
            } else if (CHECKNAME((*msg), "Takeoff")) {
                double takeoffAlt;
                b = deSerializeReal(false,&takeoffAlt,b);
                //OS_printf("Takeoff off to:%f\n",takeoffAlt);
                mavlink_msg_command_long_pack(255, 0, msgMavlink, 1, 0, MAV_CMD_NAV_TAKEOFF, 0, 0, 0, 0, 0, 0, 0, (float)takeoffAlt);
                break;
            } else if (CHECKNAME((*msg), "SetMode")) {
                //OS_printf("Setting mode\n");

                char modeName[15];
                memset(modeName,0,15);
                b = deSerializeString(modeName,b);

                if (!strcmp(modeName, "PASSIVE")) {
                    mode = AUTO;
                } else if (!strcmp(modeName, "ACTIVE")) {
                    mode = GUIDED;
                }
                mavlink_msg_set_mode_pack(255, 0, msgMavlink, 0, 1, mode);

                intfServiceResponse.sType = _command_return_;
                const bool val = true;
                serializeBool(false,val,intfServiceResponse.buffer);
                SendSBMsg(intfServiceResponse);

                break;
            } else if (CHECKNAME((*msg), "Land")) {
                mavlink_msg_command_long_pack(255, 0, msgMavlink, 1, 0, MAV_CMD_NAV_LAND, 0, 0, 0, 0, 0, 0, 0, 0);
                break;
            } else if (CHECKNAME((*msg), "SetNextMissionWP")) {
                int32_t tempSeq;
                b = deSerializeInt(false,&tempSeq,b);
                int32_t seq = -1;
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
                //OS_printf("Setting next mission WP %d\n",seq);
                break;
            } else if (CHECKNAME((*msg), "SetPos")) {
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
            } else if (CHECKNAME((*msg), "SetVel")) {

                double _velocity[3];
                b = deSerializeRealArray(_velocity,b);
                double track = _velocity[0];
                double groundspeed =_velocity[1];
                double verticalspeed = _velocity[2];


                double vn = groundspeed*cos(track*M_PI/180);
                double ve = groundspeed*sin(track*M_PI/180);
                double vu = verticalspeed;
                //OS_printf("Setting velocities: %f,%f,%f\n",vn,ve,vu);
                mavlink_msg_set_position_target_local_ned_pack(255, 0, msgMavlink, 0, 1, 0, MAV_FRAME_LOCAL_NED,
                                                               0b0000111111000111, 0, 0, 0,
                                                               (float) vn, (float) ve, (float) vu,
                                                               0, 0, 0, 0, 0);
                break;
            } else if (CHECKNAME((*msg), "SETYAW")) {
                double heading;
                int32_t relative;
                b = deSerializeReal(false,&heading,b);
                b = deSerializeInt(false,&relative,b);
                mavlink_msg_command_long_pack(255, 0, msgMavlink, 1, 0, MAV_CMD_CONDITION_YAW, 0,
                                              (float) heading, 0, 1, (float) relative, 0, 0, 0);
                break;
            } else if (CHECKNAME((*msg), "SETSPEED")) {
                double speed;
                deSerializeReal(false,&speed,b);
                mavlink_msg_command_long_pack(255, 0, msgMavlink, 1, 0, MAV_CMD_DO_CHANGE_SPEED, 0,
                                              1, (float) speed, 0, 0, 0, 0, 0);
                break;
            } else if(CHECKNAME((*msg), "Status")){
                char buffer[50];
                memset(buffer,0,50);
                b = deSerializeString(buffer,b);
                mavlink_message_t statusMsg;
                mavlink_msg_statustext_pack(2,0,&statusMsg,MAV_SEVERITY_WARNING,buffer);
                writePort(&appdataInt.gs,&statusMsg);
            }
            else{
                send =false;
            }
            break;
        }

        default:{

        };
    }

    return send;
}