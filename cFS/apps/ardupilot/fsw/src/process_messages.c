/*
 * process_messages.c
 *
 *
 */

#include <Icarous_msg.h>
#include "ardupilot_table.h"


#define EXTERN extern
#include "ardupilot.h"

void ProcessAPMessage(mavlink_message_t message){
	switch(message.msgid){
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
			position.vx = (double)globalPositionInt.vx/100;
			position.vy = (double)globalPositionInt.vy/100;
			position.vz = (double)globalPositionInt.vz/100;
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
			 switch(msg.command){
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

			 if(send){
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

			 if(appdataInt.waypoint_type[(int)msg.seq] == MAV_CMD_NAV_WAYPOINT ||
					 appdataInt.waypoint_type[(int)msg.seq] == MAV_CMD_NAV_SPLINE_WAYPOINT){
				 wpreached.reachedwaypoint = msg.seq;
				 CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &wpreached);
				 CFE_SB_SendMsg((CFE_SB_Msg_t *) &wpreached);
				 appdataInt.nextWaypointIndex = appdataInt.waypoint_index[msg.seq]+1;
			 }

			 break;
		 }

		 case MAVLINK_MSG_ID_HEARTBEAT:
		 {
			 mavlink_heartbeat_t msg;
			 mavlink_msg_heartbeat_decode(&message,&msg);
			 if(!appdataInt.foundUAV){
				 mavlink_message_t msg;
				 mavlink_msg_request_data_stream_pack(255,0,&msg,1,0,MAV_DATA_STREAM_ALL,0,1);
				 appdataInt.foundUAV = 1;
			 }
			 break;
		 }

		case MAVLINK_MSG_ID_MISSION_ITEM:
		{
			//printf("AP: MAVLINK_MSG_ID_MISSION_ITEM\n");
			break;
		}

		case MAVLINK_MSG_ID_MISSION_REQUEST_LIST:
		{
			//printf("AP: MAVLINK_MSG_ID_MISSION_REQUEST_LIST\n");
			break;
		}

		case MAVLINK_MSG_ID_MISSION_REQUEST:
		{
			//printf("AP: MAVLINK_MSG_ID_MISSION_REQUEST\n");
			break;
		}
	}
}


bool ProcessGSMessage(mavlink_message_t message){
	bool send2ap = true;
	switch(message.msgid){

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

            NoArgsCmd_t resetFpIcarous;
            CFE_SB_InitMsg(&resetFpIcarous,ICAROUS_RESETFP_MID,sizeof(NoArgsCmd_t),TRUE);
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
                    wpdata.totalWayPoints = appdataInt.numWaypoints;
                    wpdata.wayPointIndex = appdataInt.waypointSeq;
                    wpdata.latitude = msg.x;
                    wpdata.longitude = msg.y;
                    wpdata.altitude = msg.z;
                    CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &wpdata);
                    CFE_SB_SendMsg((CFE_SB_Msg_t *) &wpdata);
                    appdataInt.waypointSeq++;
                }
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

			if(msg.command == MAV_CMD_MISSION_START){
                send2ap = false;
				if(appdataInt.numWaypoints > 1) {
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
			else if(msg.command == MAV_CMD_DO_FENCE_ENABLE){

				gfdata.index = msg.param2;
				gfdata.type = msg.param3;
				gfdata.totalvertices = msg.param4;
				gfdata.floor = msg.param5;
				gfdata.ceiling = msg.param6;

				mavlink_message_t fetchfence;
				mavlink_msg_fence_fetch_point_pack(1,1,&fetchfence,255,0,0);
				writePort(&appdataInt.gs,&fetchfence);
			}
			else if(msg.command == MAV_CMD_SPATIAL_USER_1){
				traffic.index = msg.param1;
				traffic.latitude = msg.param5;
				traffic.longitude = msg.param6;
				traffic.altiude = msg.param7;
				traffic.vx = msg.param2;
				traffic.vy = msg.param3;
				traffic.vz = msg.param4;

				CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &traffic);
				CFE_SB_SendMsg((CFE_SB_Msg_t *) &traffic);
				send2ap = false;
			}
			else if(msg.command == MAV_CMD_USER_1){
				CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &resetIcarous);
				CFE_SB_SendMsg((CFE_SB_Msg_t *) &resetIcarous);
                appdataInt.nextWaypointIndex = 1000;
				send2ap = false;

                mavlink_message_t statusMsg;;
				mavlink_msg_statustext_pack(2,0,&statusMsg,MAV_SEVERITY_WARNING,"IC:Resetting Icarous");
				writePort(&appdataInt.gs,&statusMsg);
			}else if(msg.command == MAV_CMD_USER_5){
				NoArgsCmd_t ditchCmd;
				CFE_SB_InitMsg(&ditchCmd,ICAROUS_COMMANDS_MID, sizeof(NoArgsCmd_t),TRUE);
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
			//writePort(&appdataInt.ap,&message);
			break;
		}

		case MAVLINK_MSG_ID_SET_MODE:
		{
			//printf("MAVLINK_MSG_ID_SET_MODE\n");
			mavlink_set_mode_t msg;
			mavlink_msg_set_mode_decode(&message, &msg);
			//writePort(&appdataInt.ap,&message);
			break;
		}


		case MAVLINK_MSG_ID_FENCE_POINT:
		{
			//printf("MAVLINK_MSG_ID_FENCE_POINT\n");
			mavlink_fence_point_t msg;
			mavlink_msg_fence_point_decode(&message,&msg);
			int count = msg.idx;
			int total = msg.count;

			gfdata.vertexIndex = msg.idx;
			gfdata.latitude = msg.lat;
			gfdata.longitude = msg.lng;
			CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &gfdata);
			CFE_SB_SendMsg((CFE_SB_Msg_t *) &gfdata);

			if(count < total-1){
				mavlink_message_t fetchfence;
				mavlink_msg_fence_fetch_point_pack(1,1,&fetchfence,255,0,count+1);
				writePort(&appdataInt.gs,&fetchfence);
			}else{
				mavlink_message_t ack;
                mavlink_msg_command_ack_pack(1,0,&ack,MAV_CMD_DO_FENCE_ENABLE,MAV_RESULT_ACCEPTED);
				writePort(&appdataInt.gs,&ack);
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

void ARDUPILOT_ProcessPacket(){
	CFE_SB_MsgId_t  MsgId;

	ArgsCmd_t* cmd;

	MsgId = CFE_SB_GetMsgId(appdataInt.INTERFACEMsgPtr);
	switch (MsgId)
	{
		case ICAROUS_COMMANDS_MID:
		{
			cmd = (ArgsCmd_t*) appdataInt.INTERFACEMsgPtr;
			mavlink_message_t msg;
			control_mode_t mode;
			switch(cmd->name){

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
					if(cmd->param1 == _PASSIVE_){
						 mode = AUTO;
					}else if(cmd->param1 == _ACTIVE_){
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
					int tempSeq = (int)cmd->param1;
					int seq = -1;
					appdataInt.nextWaypointIndex = tempSeq;
					for(int i=0;i<=tempSeq;i++){
						seq++;
						int val = (appdataInt.waypoint_type[seq] == MAV_CMD_NAV_WAYPOINT) ||
								  (appdataInt.waypoint_type[seq] == MAV_CMD_NAV_SPLINE_WAYPOINT);
						if (!val){
							i = i-1 ;
						}
					}
					mavlink_msg_mission_set_current_pack(255,0,&msg,1,0,seq);
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

                default:{

                }

			}

			writePort(&appdataInt.ap,&msg);

			break;
		}

		case ICAROUS_VISBAND_MID:
		{
			mavlink_message_t msg;
			visbands_t* bands = (visbands_t*) appdataInt.INTERFACEMsgPtr;
			mavlink_msg_icarous_kinematic_bands_pack(1,0,&msg,bands->numBands,
					bands->type1,bands->min1,bands->max1,
					bands->type2,bands->min2,bands->max2,
					bands->type3,bands->min3,bands->max3,
					bands->type4,bands->min4,bands->max4,
					bands->type5,bands->min5,bands->max5);

			writePort(&appdataInt.gs,&msg);
			break;
		}

		case SERVICE_INTERFACE_MID: {
            mavlink_message_t msg;
			bool send = IntfServiceHandler(&msg);
            if(send) {
                writePort(&appdataInt.ap, &msg);
            }
            break;
		}

        case ICAROUS_STATUS_MID:{
            status_t* statusMsg = (status_t*) appdataInt.INTERFACEMsgPtr;
            mavlink_message_t msg;
            mavlink_msg_statustext_pack(2,0,&msg,MAV_SEVERITY_WARNING,statusMsg->buffer);
            writePort(&appdataInt.gs,&msg);
            break;
        }

        case ICAROUS_POSITION_MID:{
            position_t* position = (position_t*) appdataInt.INTERFACEMsgPtr;


            if (position->aircraft_id != CFE_PSP_GetSpacecraftId()){

                mavlink_message_t msg;
                mavlink_msg_command_long_pack(1,0,&msg,255,0,MAV_CMD_SPATIAL_USER_3,0,position->aircraft_id,
                position->vy,position->vx,position->vz,position->latitude,position->longitude,position->altitude_rel);
                writePort(&appdataInt.gs,&msg);

            }
        }
	}

	return;
}

