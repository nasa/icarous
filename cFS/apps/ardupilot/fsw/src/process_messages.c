/*
 * process_messages.c
 *
 *
 */


#define EXTERN extern

#include <msgdef/traffic_msg.h>
#include <msgdef/ardupilot_msg.h>
#include "ardupilot.h"
#include "UtilFunctions.h"

void ProcessAPMessage(mavlink_message_t message) {
	switch (message.msgid) {
		case MAVLINK_MSG_ID_GLOBAL_POSITION_INT:
		{
			mavlink_global_position_int_t globalPositionInt;
			mavlink_msg_global_position_int_decode(&message,&globalPositionInt);
			position.aircraft_id = CFE_PSP_GetSpacecraftId();
			position.time_gps  = (double)globalPositionInt.time_boot_ms/1E3;
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

			if (appdataInt.waypoint_type[(int)msg.seq] == MAV_CMD_NAV_WAYPOINT ||
					appdataInt.waypoint_type[(int)msg.seq] == MAV_CMD_NAV_SPLINE_WAYPOINT) {
				wpreached.reachedwaypoint = (uint8_t)(appdataInt.waypoint_index[msg.seq]);
				wpreached.feedback = true;
				CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &wpreached);
				CFE_SB_SendMsg((CFE_SB_Msg_t *) &wpreached);
				appdataInt.nextWaypointIndex = appdataInt.waypoint_index[msg.seq]+1;

			}
			break;
		}

		case MAVLINK_MSG_ID_MISSION_ACK:{
			if(appdataInt.startWPUplink){
				appdataInt.startWPUplink = false;
				appdataInt.startWPDownlink = true;


				mavlink_message_t msg;
				mavlink_msg_mission_request_list_pack(255,0,&msg,1,0,MAV_MISSION_TYPE_MISSION);
				writePort(&appdataInt.ap,&msg);
			}
			break;
		}

		case MAVLINK_MSG_ID_MISSION_REQUEST:{
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
				writePort(&(appdataInt.ap),&missionItem);
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
				writePort(&(appdataInt.ap),&msg);
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

					flightplan_t fp;
					CFE_SB_InitMsg(&fp,DOWNLINK_FLIGHTPLAN_MID,sizeof(flightplan_t),TRUE);
					ConvertMissionItemsToPlan(appdataInt.numDownlinkWaypoints,appdataInt.DownlinkMissionItems,&fp);
					SendSBMsg(fp);
				}else{
				    mavlink_message_t request;
				    mavlink_msg_mission_request_pack(255,0,&request,1,0,(uint16_t )(missionItem.seq + 1),MAV_MISSION_TYPE_MISSION);
				    writePort(&appdataInt.ap,&request);
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
			break;
		}

		case MAVLINK_MSG_ID_ADSB_VEHICLE:
		{

			mavlink_adsb_vehicle_t msg;
			mavlink_msg_adsb_vehicle_decode(&message,&msg);
            traffic.index = msg.ICAO_address;
            traffic.latitude = msg.lat/1.0E7;
            traffic.longitude = msg.lon/1.0E7;
            traffic.altitude = msg.altitude/1.0E3;


			double track = msg.heading/1.0E2;
			double groundspeed = msg.hor_velocity/1.0E2;
			double verticalspeed = msg.ver_velocity/1.0E2;

			double vn = groundspeed*cos(track*M_PI/180);
			double ve = groundspeed*sin(track*M_PI/180);
			double vu = verticalspeed;
            traffic.vn = vn;
            traffic.ve = ve;
            traffic.vd = vu;

            CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &traffic);
            CFE_SB_SendMsg((CFE_SB_Msg_t *) &traffic);
			break;

		}
	}
}


bool ProcessGSMessage(mavlink_message_t message) {
	bool send2ap = true;
	switch (message.msgid) {

		case MAVLINK_MSG_ID_MISSION_COUNT:
		{
			//printf("MAVLINK_MSG_ID_MISSION_COUNT\n");
			mavlink_mission_count_t msg;
			mavlink_msg_mission_count_decode(&message, &msg);
			//writePort(&appdataInt.ap,&message);
			appdataInt.numWaypoints = msg.count;
			appdataInt.waypointSeq = 0;
			appdataInt.nextWaypointIndex = 0;
			free((void*)appdataInt.waypoint_type);
			free((void*)appdataInt.waypoint_index);
			appdataInt.waypoint_type = (int*)malloc(sizeof(int)*appdataInt.numWaypoints);
			appdataInt.waypoint_index = (int*)malloc(sizeof(int)*appdataInt.numWaypoints);

			noArgsCmd_t resetFpIcarous;
			CFE_SB_InitMsg(&resetFpIcarous,ICAROUS_RESETFP_MID,sizeof(noArgsCmd_t),TRUE);
			CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &resetFpIcarous);
			CFE_SB_SendMsg((CFE_SB_Msg_t *) &resetFpIcarous);
			break;
		}

		case MAVLINK_MSG_ID_MISSION_ITEM: {
			//printf("MAVLINK_MSG_ID_MISSION_ITEM\n");
			mavlink_mission_item_t msg;
			mavlink_msg_mission_item_decode(&message, &msg);
			//writePort(&appdataInt.ap,&message);
			if (appdataInt.numWaypoints > 0) {
				appdataInt.waypoint_type[(int) msg.seq] = msg.command;
				appdataInt.waypoint_index[(int) msg.seq] = appdataInt.waypointSeq;
				if (msg.command == MAV_CMD_NAV_WAYPOINT || msg.command == MAV_CMD_NAV_SPLINE_WAYPOINT) {
					// Send message to SB

					fpdata.waypoints[appdataInt.waypointSeq].index = appdataInt.waypointSeq;
					fpdata.waypoints[appdataInt.waypointSeq].latitude = msg.x;
					fpdata.waypoints[appdataInt.waypointSeq].longitude = msg.y;
					fpdata.waypoints[appdataInt.waypointSeq].altitude = msg.z;

                    if(appdataInt.waypointSeq > 0) {
                    	fpdata.waypoints[appdataInt.waypointSeq].wp_metric = WP_METRIC_SPEED;
						fpdata.waypoints[appdataInt.waypointSeq].value_to_next_wp = fpdata.waypoints[
								appdataInt.waypointSeq - 1].value_to_next_wp;
					}
					else {
                        fpdata.waypoints[appdataInt.waypointSeq].wp_metric = WP_METRIC_SPEED;
						fpdata.waypoints[appdataInt.waypointSeq].value_to_next_wp = 1;
					}

					appdataInt.waypointSeq++;
				}else if(msg.command == MAV_CMD_DO_CHANGE_SPEED){
					fpdata.waypoints[appdataInt.waypointSeq-1].wp_metric = WP_METRIC_SPEED;
				    fpdata.waypoints[appdataInt.waypointSeq-1].value_to_next_wp = msg.param2;
                }
			}

			if (msg.seq == appdataInt.numWaypoints - 1) {
				fpdata.num_waypoints = appdataInt.waypointSeq;
                SendSBMsg(fpdata);
		    }

			break;
		}
		
		case MAVLINK_MSG_ID_MISSION_ITEM_INT:
		{
			mavlink_mission_item_int_t msg;
			mavlink_msg_mission_item_decode(&message, &msg);
			if (appdataInt.numWaypoints > 0) {
				appdataInt.waypoint_type[(int) msg.seq] = msg.command;
				appdataInt.waypoint_index[(int) msg.seq] = appdataInt.waypointSeq;
				if (msg.command == MAV_CMD_NAV_WAYPOINT || msg.command == MAV_CMD_NAV_SPLINE_WAYPOINT) {
					// Send message to SB


					fpdata.waypoints[appdataInt.waypointSeq].latitude = msg.x/1.0E7;
					fpdata.waypoints[appdataInt.waypointSeq].longitude = msg.y/1.0E7;
					fpdata.waypoints[appdataInt.waypointSeq].altitude = msg.z/1.0E7;
					appdataInt.waypointSeq++;
				}else if(msg.command == MAV_CMD_DO_CHANGE_SPEED){
				    fpdata.waypoints[appdataInt.waypointSeq-1].value_to_next_wp= msg.param2;
				}
			}

            if (msg.seq == appdataInt.numWaypoints - 1) {
				fpdata.num_waypoints = appdataInt.waypointSeq;
                SendSBMsg(fpdata);
		    }

			break;
		}

		case MAVLINK_MSG_ID_MISSION_REQUEST_LIST:
		{
			//printf("MAVLINK_MSG_ID_MISSION_REQUEST_LIST\n");
			mavlink_mission_request_list_t msg;
			mavlink_msg_mission_request_list_decode(&message, &msg);
			//writePort(&appdataInt.ap,&message);
			break;
		}

		case MAVLINK_MSG_ID_MISSION_REQUEST:
		{
			//printf("MAVLINK_MSG_ID_MISSION_REQUEST\n");
			mavlink_mission_request_t msg;
			mavlink_msg_mission_request_decode(&message, &msg);
			//writePort(&appdataInt.ap,&message);
			break;
		}


		case MAVLINK_MSG_ID_PARAM_REQUEST_LIST:
		{
			//printf("MAVLINK_MSG_ID_PARAM_REQUEST_LIST\n");
			mavlink_param_request_list_t msg;
			mavlink_msg_param_request_list_decode(&message, &msg);
			//writePort(&appdataInt.ap,&message);
			break;
		}

		case MAVLINK_MSG_ID_PARAM_REQUEST_READ:
		{
			//printf("MAVLINK_MSG_ID_PARAM_REQUEST_READ\n");
			mavlink_param_request_read_t msg;
			mavlink_msg_param_request_read_decode(&message, &msg);
			//writePort(&appdataInt.ap,&message);
			break;
		}

		case MAVLINK_MSG_ID_PARAM_VALUE:
		{
			//printf("MAVLINK_MSG_ID_PARAM_SET\n");
			mavlink_param_value_t msg;
			mavlink_msg_param_value_decode(&message, &msg);
			//writePort(&appdataInt.ap,&message);
			break;
		}

		case MAVLINK_MSG_ID_PARAM_SET:
		{
			//printf("MAVLINK_MSG_ID_PARAM_SET\n");
			mavlink_param_set_t msg;
			mavlink_msg_param_set_decode(&message, &msg);
			//writePort(&appdataInt.ap,&message);
			break;
		}

		case MAVLINK_MSG_ID_COMMAND_LONG:
		{
			//printf("MAVLINK_MSG_ID_COMMAND_LONG\n");
			mavlink_command_long_t msg;
			mavlink_msg_command_long_decode(&message, &msg);

			if (msg.command == MAV_CMD_MISSION_START) {
				send2ap = false;
				if (appdataInt.numWaypoints > 1) {
					startMission.param1 = msg.param1;
					CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &startMission);
					CFE_SB_SendMsg((CFE_SB_Msg_t *) &startMission);

					mavlink_message_t statusMsg;
					mavlink_msg_statustext_pack(2,0,&statusMsg,MAV_SEVERITY_WARNING,"IC:Starting Mission");
					writePort(&appdataInt.gs,&statusMsg);
				}else{
					mavlink_message_t statusMsg;
					mavlink_msg_statustext_pack(2,0,&statusMsg,MAV_SEVERITY_WARNING,"IC:No Flight Plan loaded");
					writePort(&appdataInt.gs,&statusMsg);

				}
			}
			else if (msg.command == MAV_CMD_DO_FENCE_ENABLE) {

				gfdata.index = (uint16_t)msg.param2;
				gfdata.type = (uint8_t)msg.param3;
				gfdata.totalvertices = (uint16_t)msg.param4;
				gfdata.floor = msg.param5;
				gfdata.ceiling = msg.param6;

				mavlink_message_t fetchfence;
				mavlink_msg_fence_fetch_point_pack(1,1,&fetchfence,255,0,0);
				writePort(&appdataInt.gs,&fetchfence);
			}
			else if (msg.command == MAV_CMD_SPATIAL_USER_1) {
				traffic.index = (uint32_t)msg.param1;
				traffic.latitude = msg.param5;
				traffic.longitude = msg.param6;
				traffic.altitude = msg.param7;
				traffic.vn = msg.param2;
				traffic.ve = msg.param3;
				traffic.vd = msg.param4;

				CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &traffic);
				CFE_SB_SendMsg((CFE_SB_Msg_t *) &traffic);
				send2ap = false;
			}
			else if (msg.command == MAV_CMD_USER_1) {
				CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &resetIcarous);
				CFE_SB_SendMsg((CFE_SB_Msg_t *) &resetIcarous);
				appdataInt.nextWaypointIndex = 1000;
				send2ap = false;

				mavlink_message_t statusMsg;;
				mavlink_msg_statustext_pack(2,0,&statusMsg,MAV_SEVERITY_WARNING,"IC:Resetting Icarous");
				writePort(&appdataInt.gs,&statusMsg);
			}else if(msg.command == MAV_CMD_USER_2){
				argsCmd_t trackCmd;
				CFE_SB_InitMsg(&trackCmd,ICAROUS_TRACK_STATUS_MID, sizeof(argsCmd_t),TRUE);
				trackCmd.param1 = (float) msg.param1;
				SendSBMsg(trackCmd);
				send2ap = false;
			} if (msg.command == MAV_CMD_USER_5) {
				noArgsCmd_t ditchCmd;
				CFE_SB_InitMsg(&ditchCmd,ICAROUS_DITCH_MID, sizeof(noArgsCmd_t),TRUE);
				ditchCmd.name = _DITCH_;
				CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &ditchCmd);
				CFE_SB_SendMsg((CFE_SB_Msg_t *) &ditchCmd);
				send2ap = false;
			}
			else {
				//writePort(&appdataInt.ap,&message);
			}

			break;
		}

		case MAVLINK_MSG_ID_COMMAND_INT:
		{
			//printf("MAVLINK_MSG_ID_COMMAND_INT\n");
			mavlink_command_int_t msg;
			mavlink_msg_command_int_decode(&message, &msg);
			break;
		}

		case MAVLINK_MSG_ID_SET_MODE:
		{
			//printf("MAVLINK_MSG_ID_SET_MODE\n");
			mavlink_set_mode_t msg;
			mavlink_msg_set_mode_decode(&message, &msg);
			break;
		}


		case MAVLINK_MSG_ID_FENCE_POINT:
		{
			//printf("MAVLINK_MSG_ID_FENCE_POINT\n");
			mavlink_fence_point_t msg;
			mavlink_msg_fence_point_decode(&message,&msg);
			int count = msg.idx;
			int total = msg.count;

			gfdata.vertices[msg.idx][0] = msg.lat;
            gfdata.vertices[msg.idx][1] = msg.lng;


			if (count < total-1) {
				mavlink_message_t fetchfence;
				mavlink_msg_fence_fetch_point_pack(1,1,&fetchfence,255,0,count+1);
				writePort(&appdataInt.gs,&fetchfence);
			}else{
				mavlink_message_t ack;
				mavlink_msg_command_ack_pack(1,0,&ack,MAV_CMD_DO_FENCE_ENABLE,MAV_RESULT_ACCEPTED);
				writePort(&appdataInt.gs,&ack);
			}

		    if(count == total - 1) {
				CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &gfdata);
				CFE_SB_SendMsg((CFE_SB_Msg_t *) &gfdata);
			}

			break;
		}

		case MAVLINK_MSG_ID_RADIO:
		{
			//writePort(&appdataInt.ap,&message);
			break;
		}

		case MAVLINK_MSG_ID_RADIO_STATUS:
		{
			//writePort(&appdataInt.ap,&message);
			break;
		}
	}

	return send2ap;
}

void ARDUPILOT_ProcessPacket() {
	CFE_SB_MsgId_t  MsgId;

	argsCmd_t* cmd;

	MsgId = CFE_SB_GetMsgId(appdataInt.INTERFACEMsgPtr);
	switch (MsgId)
	{

		case ICAROUS_BANDS_TRACK_MID:
		{
			mavlink_message_t msg;
			bands_t* bands = (bands_t*) appdataInt.INTERFACEMsgPtr;
			mavlink_msg_icarous_kinematic_bands_pack(1,0,&msg,(int8_t)bands->numBands,
                    (uint8_t)bands->type[0],(float)bands->min[0],(float)bands->max[0],
					(uint8_t)bands->type[1],(float)bands->min[1],(float)bands->max[1],
                    (uint8_t)bands->type[2],(float)bands->min[2],(float)bands->max[2],
                    (uint8_t)bands->type[3],(float)bands->min[3],(float)bands->max[3],
                    (uint8_t)bands->type[4],(float)bands->min[4],(float)bands->max[4]);

			if(bands->numBands > 0)
                writePort(&appdataInt.gs,&msg);
			break;
		}

		case UPLINK_FLIGHTPLAN_MID:{
			//OS_printf("received flight plan\n");
			mavlink_message_t msg;
			flightplan_t* fp = (flightplan_t*)appdataInt.INTERFACEMsgPtr;

			uint16_t  numMissionItems = ConvertPlanToMissionItems(fp);
			appdataInt.startWPUplink = true;
			appdataInt.numUplinkWaypoints = numMissionItems;
			mavlink_msg_mission_count_pack(255,0,&msg,1,0,numMissionItems,MAV_MISSION_TYPE_MISSION);
			writePort(&appdataInt.ap,&msg);
		}

		case ICAROUS_STATUS_MID:{
			status_t* statusMsg = (status_t*) appdataInt.INTERFACEMsgPtr;
			mavlink_message_t msg;
			mavlink_msg_statustext_pack(2,0,&msg,MAV_SEVERITY_WARNING,statusMsg->buffer);
			writePort(&appdataInt.gs,&msg);
			break;
		}

        case ICAROUS_COMMANDS_MID:
        {
            cmd = (argsCmd_t*) appdataInt.INTERFACEMsgPtr;
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
                    }else if ((int)cmd->param1 == _ACTIVE_) {
                        mode = GUIDED;
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
                    int seq = -1;
                    appdataInt.nextWaypointIndex = tempSeq;
                    for(int i=0;i<=tempSeq;i++) {
                        seq++;
						i = (appdataInt.waypoint_type[seq] == MAV_CMD_NAV_WAYPOINT)?i:i-1;
                    }
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
					writePort(&appdataInt.gs,&msg);
					break;
				}
                default:{

                }

            }

            writePort(&appdataInt.ap,&msg);

            break;
        }

	}

	return;
}

uint16_t ConvertPlanToMissionItems(flightplan_t* fp){
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

	return count;
}

void ConvertMissionItemsToPlan(uint16_t  size, mavlink_mission_item_t items[],flightplan_t* fp){

	int count = 0;
	for(int i=0;i<size;++i){
		switch(items[i].command){

			case MAV_CMD_NAV_WAYPOINT:{
				fp->waypoints[count].latitude = items[i].x;
				fp->waypoints[count].longitude = items[i].y;
				fp->waypoints[count].altitude = items[i].z;
				count++;
				//OS_printf("constructed waypoint\n");
				break;
			}

			case MAV_CMD_NAV_LOITER_TIME:{
				fp->waypoints[count].latitude = items[i].x;
				fp->waypoints[count].longitude = items[i].y;
				fp->waypoints[count].altitude = items[i].z;
				if(items[i].command == MAV_CMD_NAV_LOITER_TIME){
					fp->waypoints[count-1].value_to_next_wp = items[i].param1;

				}
				count++;
				//OS_printf("Setting loiter point %f\n",items[i].param1);

				break;
			}

			case MAV_CMD_DO_CHANGE_SPEED:{
				if(i>0 && i < size-1) {
					double wpA[3] = {items[i - 1].x, items[i - 1].y, items[i - 1].z};
					double wpB[3] = {items[i + 1].x,items[i + 1].y,items[i + 1].z};
					double dist = ComputeDistance(wpA,wpB);
					double eta = dist/items[i].param2;

					fp->waypoints[count-1].value_to_next_wp = eta;
					//OS_printf("Setting ETA to %f\n",eta);
				}
				break;
			}
		}
	}

	fp->num_waypoints = count;
}
