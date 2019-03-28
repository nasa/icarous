/*
 * ardupilot_process_messages.c
 *
 *
 */
#define EXTERN extern

#include <msgdef/traffic_msg.h>
#include <msgdef/ardupilot_msg.h>
#include "ardupilot.h"
#include "UtilFunctions.h"

int GetMAVLinkMsgFromAP(){
    int n = readPort(&appdataInt.ap);
    mavlink_message_t message;
    mavlink_status_t status;
    uint8_t msgReceived = 0;
    for(int i=0;i<n;i++){
        uint8_t cp = appdataInt.ap.recvbuffer[i];
        msgReceived = mavlink_parse_char(MAVLINK_COMM_0, cp, &message, &status);
        if(msgReceived){
            // Send SB message if necessary
            ProcessAPMessage(message);
        }
    }
    return n;
}


void apSendHeartbeat(){
    mavlink_message_t hbeat;
    mavlink_msg_heartbeat_pack(1,0,&hbeat,MAV_TYPE_ONBOARD_CONTROLLER,MAV_AUTOPILOT_INVALID,0,0,0);
    writeMavlinkData(&appdataInt.ap,&hbeat);
}

void ProcessAPMessage(mavlink_message_t message) {
    switch (message.msgid) {


        case MAVLINK_MSG_ID_MISSION_REQUEST_INT:
        {
            //printf("MAVLINK_MSG_ID_MISSION_REQUEST_INT\n");
            mavlink_mission_request_int_t msg;
            mavlink_msg_mission_request_int_decode(&message, &msg);


            mavlink_message_t msgMissionItemInt;
            mavlink_msg_mission_item_int_pack(1,0,&msgMissionItemInt,255,0,msg.seq,appdataInt.UplinkMissionItems[msg.seq].frame,
                                              appdataInt.UplinkMissionItems[msg.seq].command,
                                              appdataInt.UplinkMissionItems[msg.seq].current,
                                              appdataInt.UplinkMissionItems[msg.seq].autocontinue,
                                              appdataInt.UplinkMissionItems[msg.seq].param1,
                                              appdataInt.UplinkMissionItems[msg.seq].param2,
                                              appdataInt.UplinkMissionItems[msg.seq].param3,
                                              appdataInt.UplinkMissionItems[msg.seq].param4,
                                              appdataInt.UplinkMissionItems[msg.seq].x,
                                              appdataInt.UplinkMissionItems[msg.seq].y,
                                              appdataInt.UplinkMissionItems[msg.seq].z,
                                              appdataInt.UplinkMissionItems[msg.seq].mission_type);

            writeMavlinkData(&appdataInt.ap,&msgMissionItemInt);
            break;
        }

        case MAVLINK_MSG_ID_MISSION_REQUEST:{
            //printf("MAVLINK_MSG_ID_MISSION_REQUEST\n");
            if(appdataInt.startWPUplink) {
                mavlink_mission_request_t msg;
                mavlink_msg_mission_request_decode(&message,&msg);

                mavlink_message_t missionItem;
                mavlink_msg_mission_item_pack(255,0,&missionItem,1,0,appdataInt.UplinkMissionItems[msg.seq].seq,
                                              appdataInt.UplinkMissionItems[msg.seq].frame,
                                              appdataInt.UplinkMissionItems[msg.seq].command,
                                              appdataInt.UplinkMissionItems[msg.seq].current,
                                              appdataInt.UplinkMissionItems[msg.seq].autocontinue,
                                              appdataInt.UplinkMissionItems[msg.seq].param1,
                                              appdataInt.UplinkMissionItems[msg.seq].param2,
                                              appdataInt.UplinkMissionItems[msg.seq].param3,
                                              appdataInt.UplinkMissionItems[msg.seq].param4,
                                              appdataInt.UplinkMissionItems[msg.seq].x,
                                              appdataInt.UplinkMissionItems[msg.seq].y,
                                              appdataInt.UplinkMissionItems[msg.seq].z,
                                              appdataInt.UplinkMissionItems[msg.seq].mission_type);
                writeMavlinkData(&(appdataInt.ap),&missionItem);
            }
            break;
        }


        case MAVLINK_MSG_ID_GLOBAL_POSITION_INT:
        {
            mavlink_global_position_int_t globalPositionInt;
            mavlink_msg_global_position_int_decode(&message,&globalPositionInt);
            position.aircraft_id = CFE_PSP_GetSpacecraftId();
            position.time_boot  = (double)globalPositionInt.time_boot_ms/1E3;
            position.latitude  = (double)globalPositionInt.lat/1E7;
            position.longitude = (double)globalPositionInt.lon/1E7;
            position.altitude_abs  = (double)globalPositionInt.alt/1E3;
            position.altitude_rel  = (double)globalPositionInt.relative_alt/1E3;
            position.vn = (double)globalPositionInt.vx/100;
            position.ve = (double)globalPositionInt.vy/100;
            position.vd = (double)globalPositionInt.vz/100;
            position.hdg = (double)globalPositionInt.hdg/100;

            CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &position);
            CFE_SB_SendMsg((CFE_SB_Msg_t *) &position);
            break;
        }

        case MAVLINK_MSG_ID_ATTITUDE:{
            mavlink_attitude_t apAttitude;
            mavlink_msg_attitude_decode(&message,&apAttitude);
            attitude.pitch = apAttitude.pitch*180/M_PI;
            attitude.roll  = apAttitude.roll*180/M_PI;
            attitude.yaw = apAttitude.yaw*180/M_PI;
            attitude.pitchspeed = apAttitude.pitchspeed*180/M_PI;
            attitude.rollspeed = apAttitude.rollspeed*180/M_PI;
            attitude.yawspeed = apAttitude.yawspeed*180/M_PI;
            attitude.time_boot = apAttitude.time_boot_ms;

            if(attitude.yaw < 0){
                attitude.yaw = 360 + attitude.yaw;
            }

            SendSBMsg(attitude);
            break;
        }

        case MAVLINK_MSG_ID_BATTERY_STATUS:{
            mavlink_battery_status_t apBatteryStatus;
            mavlink_msg_battery_status_decode(&message,&apBatteryStatus);
            battery_status.id = apBatteryStatus.id;
            battery_status.battery_function = apBatteryStatus.battery_function;
            battery_status.type = apBatteryStatus.type;
            battery_status.temperature = apBatteryStatus.temperature;
            memcpy(battery_status.voltages, apBatteryStatus.voltages, 10*sizeof(uint16_t));
            battery_status.current_battery = apBatteryStatus.current_battery;
            battery_status.current_consumed = apBatteryStatus.current_consumed;
            battery_status.energy_consumed = apBatteryStatus.energy_consumed;
            battery_status.battery_remaining = apBatteryStatus.battery_remaining;

            SendSBMsg(battery_status)

            break;
        }

        case MAVLINK_MSG_ID_COMMAND_ACK:
        {
            mavlink_command_ack_t msg;
            mavlink_msg_command_ack_decode(&message, &msg);
            uint8_t send = 0;
            switch (msg.command) {
                case MAV_CMD_COMPONENT_ARM_DISARM:
                {
                    ack.name = _ARM_;
                    ack.result = msg.result;
                    send = 1;
                    break;
                }
                case MAV_CMD_NAV_TAKEOFF:
                {
                    ack.name = _TAKEOFF_;
                    ack.result = msg.result;
                    send = 1;
                    break;
                }
            }

            if (send) {
                CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &ack);
                CFE_SB_SendMsg((CFE_SB_Msg_t *) &ack);
            }
            break;
        }


		case MAVLINK_MSG_ID_MISSION_ITEM_REACHED:
		{
		    //printf("AP: MAVLINK_MSG_ID_MISSION_ITEM_REACHED\n");
			mavlink_mission_item_reached_t msg;
			mavlink_msg_mission_item_reached_decode(&message, &msg);

			// Find the corresponding index of the flightplan_t data
		    int i;
		    bool avail = false;
		    for(i=0;i<fpdata.num_waypoints;++i)	{
		    	if (appdataInt.waypoint_index[i] == msg.seq) {
		    		avail = true;
		    		break;
		    	}
		    }

			wpreached.reachedwaypoint = (uint8_t)(i);
            wpreached.feedback = true;
            if(avail) {
				CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &wpreached);
				CFE_SB_SendMsg((CFE_SB_Msg_t *) &wpreached);
				//OS_printf("waypoint reached = %d / %d\n", i, msg.seq);
			}
			break;
		}

        case MAVLINK_MSG_ID_MISSION_ACK:{
            if(appdataInt.startWPUplink){
                mavlink_mission_ack_t msg;
                mavlink_msg_mission_ack_decode(&message,&msg);
                if (msg.type == 0)
                    appdataInt.startWPUplink = false;
            }
            break;
        }

        case MAVLINK_MSG_ID_MISSION_COUNT:{
            if(appdataInt.startWPDownlink){
                mavlink_mission_count_t missionCount;
                mavlink_msg_mission_count_decode(&message,&missionCount);
                appdataInt.numDownlinkWaypoints = missionCount.count;
                mavlink_message_t msg;
                mavlink_msg_mission_request_pack(255,0,&msg,1,0,appdataInt.downlinkRequestIndex,MAV_MISSION_TYPE_MISSION);
                //writeMavlinkData(&(appdataInt.ap),&msg);
            }
            break;
        }

        case MAVLINK_MSG_ID_MISSION_ITEM:{
            if(appdataInt.startWPDownlink){
                mavlink_mission_item_t missionItem;
                mavlink_msg_mission_item_decode(&message,&missionItem);

                memcpy(appdataInt.DownlinkMissionItems + missionItem.seq,&missionItem,sizeof(mavlink_mission_item_t));

                if(missionItem.seq == appdataInt.numDownlinkWaypoints - 1){

                    mavlink_message_t ack;
                    mavlink_msg_mission_ack_pack(255,0,&ack,1,0,MAV_MISSION_ACCEPTED,MAV_MISSION_TYPE_MISSION);
                    appdataInt.startWPDownlink = false;
                    //OS_printf("Received downlink flightplan from pixhawk\n");

                    //flightplan_t fp;
                    //CFE_SB_InitMsg(&fp,DOWNLINK_FLIGHTPLAN_MID,sizeof(flightplan_t),TRUE);
                    //ConvertMissionItemsToPlan(appdataInt.numDownlinkWaypoints,appdataInt.DownlinkMissionItems,&fp);
                    //SendSBMsg(fp);
                }else{
                    mavlink_message_t request;
                    mavlink_msg_mission_request_pack(255,0,&request,1,0,(uint16_t )(missionItem.seq + 1),MAV_MISSION_TYPE_MISSION);
                    //writeMavlinkData(&appdataInt.ap,&request);
                    //OS_printf("Requesting %d waypoint \n",missionItem.seq + 1);
                }
            }
        }

        case MAVLINK_MSG_ID_HEARTBEAT:
        {
            mavlink_heartbeat_t msg;
            mavlink_msg_heartbeat_decode(&message,&msg);
            if (!appdataInt.foundUAV) {
                mavlink_message_t msg;
                mavlink_msg_request_data_stream_pack(255,0,&msg,1,0,MAV_DATA_STREAM_ALL,0,1);
                appdataInt.foundUAV = 1;
                CFE_EVS_SendEvent(ARDUPILOT_CONNECTED_TO_AP_EID,CFE_EVS_INFORMATION,"Connection to autopilot established");
            }
            appdataInt.currentAPMode = msg.custom_mode;
            break;
        }

        case MAVLINK_MSG_ID_ADSB_VEHICLE:
        {

            mavlink_adsb_vehicle_t msg;
            mavlink_msg_adsb_vehicle_decode(&message,&msg);

            traffic.index = msg.ICAO_address;
            traffic.type = _TRAFFIC_ADSB_;
            traffic.latitude = msg.lat/1.0E7;
            traffic.longitude = msg.lon/1.0E7;
            traffic.altitude = msg.altitude/1.0E3;

            double positionA[3] = {position.latitude,position.longitude,position.altitude_rel};
            double positionB[3] = {traffic.latitude,traffic.longitude,traffic.altitude};
            double dist = ComputeDistance(positionA,positionB);

            if(dist <= 2000 && (appdataInt.currentAPMode == GUIDED || appdataInt.currentAPMode == AUTO) ) {
                double track = msg.heading / 1.0E2;
                double groundspeed = msg.hor_velocity / 1.0E2;
                double verticalspeed = msg.ver_velocity / 1.0E2;

                double vn = groundspeed * cos(track * M_PI / 180);
                double ve = groundspeed * sin(track * M_PI / 180);
                double vu = verticalspeed;
                traffic.vn = vn;
                traffic.ve = ve;
                traffic.vd = vu;

                CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &traffic);
                CFE_SB_SendMsg((CFE_SB_Msg_t *) &traffic);
            }
            break;

        }

        case MAVLINK_MSG_ID_VFR_HUD:{
            mavlink_vfr_hud_t msg;
            mavlink_msg_vfr_hud_decode(&message,&msg);

            vfrhud.alt = msg.alt;
            vfrhud.heading = msg.heading;
            vfrhud.airspeed = msg.airspeed;
            vfrhud.groundspeed = msg.groundspeed;
            vfrhud.climb = msg.climb;
            vfrhud.throttle = msg.throttle;
            vfrhud.modeAP = appdataInt.currentAPMode;
            vfrhud.modeIcarous = appdataInt.icarousMode;
            SendSBMsg(vfrhud);
            break;
        }

        case MAVLINK_MSG_ID_GPS_STATUS:{
            mavlink_gps_status_t msg;
            mavlink_msg_gps_status_decode(&message,&msg);
            position.numSats = msg.satellites_visible;
            break;
        }

        case MAVLINK_MSG_ID_GPS_RAW_INT:{
            mavlink_gps_raw_int_t msg;
            mavlink_msg_gps_raw_int_decode(&message,&msg);
            position.hdop = msg.eph;
            position.vdop = msg.epv;
            break;
        }

        case MAVLINK_MSG_ID_SYSTEM_TIME:{
            mavlink_system_time_t msg;
            mavlink_msg_system_time_decode(&message,&msg);
            position.time_gps = msg.time_unix_usec/1E6;
            break;
        }


        case MAVLINK_MSG_ID_MISSION_CURRENT:{
            mavlink_mission_current_t msg;
            mavlink_msg_mission_current_decode(&message,&msg);
            vfrhud.waypointCurrent = msg.seq;
            break;
        }
    }
}

void ARDUPILOT_ProcessPacket() {
    CFE_SB_MsgId_t  MsgId;


    MsgId = CFE_SB_GetMsgId(appdataInt.INTERFACEMsgPtr);
    switch (MsgId)
    {
        case ICAROUS_FLIGHTPLAN_MID:{
            flightplan_t* msg = (flightplan_t*)appdataInt.INTERFACEMsgPtr;
            memcpy(&fpdata,msg,sizeof(flightplan_t));
            int count = apConvertPlanToMissionItems(&fpdata);
            appdataInt.numWaypoints = count;
#ifdef SITL
            mavlink_message_t missionCount;
            mavlink_msg_mission_count_pack(200,1,&missionCount,1,0,count,MAV_MISSION_TYPE_MISSION);
            writeMavlinkData(&appdataInt.ap,&missionCount);
            appdataInt.startWPUplink = true;
#endif
            break;
        }

        case ICAROUS_COMMANDS_MID:
        {
            argsCmd_t *cmd = (argsCmd_t*) appdataInt.INTERFACEMsgPtr;
            mavlink_message_t msg;
            controlMode_e mode;
            switch (cmd->name) {

                case _ARM_:
                {
                    mavlink_msg_command_long_pack(255,0,&msg,1,0,MAV_CMD_COMPONENT_ARM_DISARM,0,cmd->param1,0,0,0,0,0,0);
                    break;
                }

                case _TAKEOFF_:
                {
                    mavlink_msg_command_long_pack(255,0,&msg,1,0,MAV_CMD_NAV_TAKEOFF,0,1,0,0,0,0,0,cmd->param1);
                    break;
                }

                case _SETMODE_:
                {
                    if ((int)cmd->param1 == _PASSIVE_) {
                        mode = AUTO;
                        appdataInt.icarousMode = 0;
                    }else if ((int)cmd->param1 == _ACTIVE_) {
                        mode = GUIDED;
                        appdataInt.icarousMode = 1;
                    }
                    mavlink_msg_set_mode_pack(255,0,&msg,0,1,mode);
                    break;
                }

                case _LAND_:
                {
                    mavlink_msg_command_long_pack(255,0,&msg,1,0,MAV_CMD_NAV_LAND,0,1,0,0,0,cmd->param5,cmd->param6,cmd->param7);
                    break;
                }

                case _GOTOWP_:
                {

                    if(appdataInt.numWaypoints == 0){
                        break;
                    }

                    int tempSeq = (int)cmd->param1;
                    int seq = appdataInt.waypoint_index[tempSeq];
                    mavlink_msg_mission_set_current_pack(255,0,&msg,1,0,seq);

                    if(tempSeq>0) {
                        wpreached.reachedwaypoint = (uint8_t) (tempSeq - 1);
                        wpreached.feedback = false;
                    }
                    SendSBMsg(wpreached);
                    break;
                }

                case _SETPOS_:
                {
                    mavlink_msg_set_position_target_global_int_pack(255,0,&msg,0,1,0,MAV_FRAME_GLOBAL_RELATIVE_ALT_INT,
                                                                    0b0000111111111000,(int)(cmd->param1*1E7),(int)(cmd->param2*1E7),(cmd->param3),
                                                                    0,0,0,0,0,0,0,0);
                    break;
                }

                case _SETVEL_:
                {
                    mavlink_msg_set_position_target_local_ned_pack(255,0,&msg,0,1,0,MAV_FRAME_LOCAL_NED, 0b0000111111000111,0,0,0,
                                                                   (float)cmd->param1,(float)cmd->param2,(float)cmd->param3,
                                                                   0,0,0,0,0);

                    break;
                }

                case _SETYAW_:
                {
                    mavlink_msg_command_long_pack(255,0,&msg,1,0,MAV_CMD_CONDITION_YAW,0,
                                                  (float)cmd->param1,(float)cmd->param2,(float)cmd->param3,(float)cmd->param4,0,0,0);
                    break;
                }

                case _SETSPEED_:
                {
                    mavlink_msg_command_long_pack(255,0,&msg,1,0,MAV_CMD_DO_CHANGE_SPEED,0,
                                                  1,(float)cmd->param1,0,0,0,0,0);
                    break;
                }

                case _STATUS_: {
                    mavlink_msg_statustext_pack(255, 0, &msg, MAV_SEVERITY_WARNING, cmd->buffer);
                    break;
                }
                default:{

                }

            }

            writeMavlinkData(&appdataInt.ap,&msg);

            break;
        }

    }

    return;
}

uint16_t apConvertPlanToMissionItems(flightplan_t* fp){
    int count = 0;
    for(int i=0;i<fp->num_waypoints;++i){
        appdataInt.UplinkMissionItems[count].target_system = 1;
        appdataInt.UplinkMissionItems[count].target_component = 0;
        appdataInt.UplinkMissionItems[count].seq = (uint16_t )count;
        appdataInt.UplinkMissionItems[count].mission_type = MAV_MISSION_TYPE_MISSION;
        appdataInt.UplinkMissionItems[count].command = MAV_CMD_NAV_WAYPOINT;
        appdataInt.UplinkMissionItems[count].frame = MAV_FRAME_GLOBAL_RELATIVE_ALT;
        appdataInt.UplinkMissionItems[count].autocontinue = 1;
        appdataInt.UplinkMissionItems[count].current = 0;
        appdataInt.UplinkMissionItems[count].x = (float)fp->waypoints[i].latitude;
        appdataInt.UplinkMissionItems[count].y = (float)fp->waypoints[i].longitude;
        appdataInt.UplinkMissionItems[count].z = (float)fp->waypoints[i].altitude;
        appdataInt.waypoint_index[i] = count;

        count++;
        if(i < fp->num_waypoints-1){
            if(fp->waypoints[i].wp_metric == WP_METRIC_ETA) {
                double currentWP[3] = {fp->waypoints[i].latitude, fp->waypoints[i].longitude,
                                       fp->waypoints[i].altitude};
                double nextWP[3] = {fp->waypoints[i + 1].latitude, fp->waypoints[i + 1].longitude,
                                    fp->waypoints[i + 1].altitude};
                double dist2NextWP = ComputeDistance(currentWP, nextWP);
                double time2NextWP = fp->waypoints[i].value_to_next_wp;
                double speed2NextWP = dist2NextWP/time2NextWP;

                if (dist2NextWP > 1E-3) {
                    appdataInt.UplinkMissionItems[count].target_system = 1;
                    appdataInt.UplinkMissionItems[count].target_component = 0;
                    appdataInt.UplinkMissionItems[count].seq = (uint16_t) count;
                    appdataInt.UplinkMissionItems[count].mission_type = MAV_MISSION_TYPE_MISSION;
                    appdataInt.UplinkMissionItems[count].command = MAV_CMD_DO_CHANGE_SPEED;
                    appdataInt.UplinkMissionItems[count].frame = MAV_FRAME_GLOBAL_RELATIVE_ALT;
                    appdataInt.UplinkMissionItems[count].autocontinue = 1;
                    appdataInt.UplinkMissionItems[count].current = 0;
                    appdataInt.UplinkMissionItems[count].param1 = 1;
                    appdataInt.UplinkMissionItems[count].param2 = speed2NextWP;
                    appdataInt.UplinkMissionItems[count].param3 = 0;
                    appdataInt.UplinkMissionItems[count].param4 = 0;
                    appdataInt.UplinkMissionItems[count].x = 0;
                    appdataInt.UplinkMissionItems[count].y = 0;
                    appdataInt.UplinkMissionItems[count].z = 0;
                    count++;
                    //OS_printf("Constructed speed waypoint:%f\n",speed2NextWP);
                }else if(time2NextWP > 0){
                    appdataInt.UplinkMissionItems[count].target_system = 1;
                    appdataInt.UplinkMissionItems[count].target_component = 0;
                    appdataInt.UplinkMissionItems[count].seq = (uint16_t) count;
                    appdataInt.UplinkMissionItems[count].mission_type = MAV_MISSION_TYPE_MISSION;
                    appdataInt.UplinkMissionItems[count].command = MAV_CMD_NAV_LOITER_TIME;
                    appdataInt.UplinkMissionItems[count].frame = MAV_FRAME_GLOBAL_RELATIVE_ALT;
                    appdataInt.UplinkMissionItems[count].autocontinue = 1;
                    appdataInt.UplinkMissionItems[count].current = 0;
                    appdataInt.UplinkMissionItems[count].param1 = time2NextWP;
                    appdataInt.UplinkMissionItems[count].param2 = 0;
                    appdataInt.UplinkMissionItems[count].param3 = 15;
                    appdataInt.UplinkMissionItems[count].param4 = 0;
                    appdataInt.UplinkMissionItems[count].x = nextWP[0];
                    appdataInt.UplinkMissionItems[count].y = nextWP[1];
                    appdataInt.UplinkMissionItems[count].z = nextWP[2];
                    ++i;
                    count++;
                    //OS_printf("Constructed loiter waypoint\n");
                }
            }
        }

    }

    appdataInt.numUplinkWaypoints = count;
    return count;
}

