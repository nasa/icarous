/*
 * process_messages.c
 *
 *
 */


#include <msgdef/ardupilot_msg.h>
#include <msgids/ardupilot_msgids.h>
#include "gsInterface.h"
#include "UtilFunctions.h"

uint8_t sysid = 1;
uint8_t compid = 1;

uint8_t target_sys = 255;
uint8_t target_comp = 0;

int GetMAVLinkMsgFromGS(){
    int n = readPort(&appdataIntGS.gs);
    mavlink_message_t message;
    mavlink_status_t status;
    uint8_t msgReceived = 0;
    for(int i=0;i<n;i++){
        uint8_t cp = appdataIntGS.gs.recvbuffer[i];
        msgReceived = mavlink_parse_char(MAVLINK_COMM_1, cp, &message, &status);
        if(msgReceived){
            ProcessGSMessage(message);
        }
    }
    return n;
}

void gsSendHeartbeat(){

    mavlink_message_t hbeat;
    mavlink_msg_heartbeat_pack(sysid,compid,&hbeat,MAV_TYPE_ONBOARD_CONTROLLER,MAV_AUTOPILOT_INVALID,appdataIntGS.currentIcarousMode,appdataIntGS.currentApMode,0);
    writeMavlinkData(&appdataIntGS.gs,&hbeat);
}

void ProcessGSMessage(mavlink_message_t message) {
    bool send2ap = true;
    switch (message.msgid) {

        case MAVLINK_MSG_ID_MISSION_COUNT:
        {
            //printf("MAVLINK_MSG_ID_MISSION_COUNT\n");
            mavlink_mission_count_t msg;
            mavlink_msg_mission_count_decode(&message, &msg);
            appdataIntGS.numWaypoints = msg.count;
            appdataIntGS.waypointSeq = 0;
            appdataIntGS.nextWaypointIndex = 0;
            appdataIntGS.receivingWP = 0;

            noArgsCmd_t resetFpIcarous;
            CFE_SB_InitMsg(&resetFpIcarous,ICAROUS_RESETFP_MID,sizeof(noArgsCmd_t),TRUE);
            CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &resetFpIcarous);
            CFE_SB_SendMsg((CFE_SB_Msg_t *) &resetFpIcarous);

            mavlink_message_t msgRequest;
            mavlink_msg_mission_request_pack(sysid,compid,&msgRequest,target_sys,target_comp,appdataIntGS.waypointSeq,MAV_MISSION_TYPE_MISSION);
            writeMavlinkData(&appdataIntGS.gs,&msgRequest);
            break;
        }

        case MAVLINK_MSG_ID_MISSION_ITEM:
        {
            //printf("MAVLINK_MSG_ID_MISSION_ITEM\n");
            mavlink_mission_item_t msg;
            mavlink_msg_mission_item_decode(&message, &msg);

            bool correct = false;
            if (appdataIntGS.receivingWP == msg.seq) {
                correct = true;
                appdataIntGS.receivingWP = msg.seq + 1;
            }

            if (correct) {
                memcpy(appdataIntGS.ReceivedMissionItems+msg.seq,&msg,sizeof(mavlink_mission_item_t));
            }

            if (msg.seq == appdataIntGS.numWaypoints - 1) {
                ConvertMissionItemsToPlan(appdataIntGS.numWaypoints,appdataIntGS.ReceivedMissionItems,&appdataIntGS.fpData);
                SendSBMsg(appdataIntGS.fpData);

                mavlink_message_t msgAck;
                mavlink_msg_mission_ack_pack(sysid, compid, &msgAck, target_sys, target_comp, MAV_MISSION_ACCEPTED, msg.mission_type);
                //printf("mission accepted\n");
                writeMavlinkData(&appdataIntGS.gs, &msgAck);
            }

            if(appdataIntGS.receivingWP < appdataIntGS.numWaypoints) {
                mavlink_message_t msgRequest;
                mavlink_msg_mission_request_pack(sysid, compid, &msgRequest, target_sys, target_comp, appdataIntGS.receivingWP,
                                                     MAV_MISSION_TYPE_MISSION);
                writeMavlinkData(&appdataIntGS.gs, &msgRequest);
            }

            break;
        }

        case MAVLINK_MSG_ID_MISSION_REQUEST_LIST:
        {
            //printf("MAVLINK_MSG_ID_MISSION_REQUEST_LIST\n");
            mavlink_mission_request_list_t msg;
            mavlink_msg_mission_request_list_decode(&message, &msg);
            int count = 0;

            if (msg.mission_type == MAV_MISSION_TYPE_MISSION)
            {
                if(appdataIntGS.numWaypoints > 0){
                     count = appdataIntGS.numWaypoints;
                }else{
                    argsCmd_t cmd;
                    CFE_SB_InitMsg(&cmd,ICAROUS_COMMANDS_MID,sizeof(argsCmd_t),TRUE);
                    cmd.name = _GETFP_;
                    SendSBMsg(cmd);
                }
            }else if(msg.mission_type == MAV_MISSION_TYPE_FENCE){
                for(int i=0;i<appdataIntGS.numGeofences;++i){
                    count += appdataIntGS.fenceVertices[i];
                }
            }else if(msg.mission_type == MAV_MISSION_TYPE_RALLY){
                count = appdataIntGS.trajectory.num_waypoints;
            }

            if(count > 0){
                mavlink_message_t msgCount;
                mavlink_msg_mission_count_pack(sysid, compid, &msgCount, target_sys, target_comp, count, msg.mission_type);
                writeMavlinkData(&appdataIntGS.gs, &msgCount);
            }

            break;
        }

        case MAVLINK_MSG_ID_MISSION_REQUEST:{
            //printf("MAVLINK_MSG_ID_MISSION_REQUEST\n");
            mavlink_mission_request_t msg;
            mavlink_msg_mission_request_decode(&message, &msg);


            mavlink_message_t msgMissionItem;
            if(msg.mission_type == MAV_MISSION_TYPE_MISSION){
                mavlink_msg_mission_item_pack(sysid,compid,&msgMissionItem,target_sys,target_comp,msg.seq,appdataIntGS.ReceivedMissionItems[msg.seq].frame,
                                                                               appdataIntGS.ReceivedMissionItems[msg.seq].command,
                                                                               appdataIntGS.ReceivedMissionItems[msg.seq].current,
                                                                               appdataIntGS.ReceivedMissionItems[msg.seq].autocontinue,
                                                                               appdataIntGS.ReceivedMissionItems[msg.seq].param1,
                                                                               appdataIntGS.ReceivedMissionItems[msg.seq].param2,
                                                                               appdataIntGS.ReceivedMissionItems[msg.seq].param3,
                                                                               appdataIntGS.ReceivedMissionItems[msg.seq].param4,
                                                                               appdataIntGS.ReceivedMissionItems[msg.seq].x,
                                                                               appdataIntGS.ReceivedMissionItems[msg.seq].y,
                                                                               appdataIntGS.ReceivedMissionItems[msg.seq].z,
                                                                               appdataIntGS.ReceivedMissionItems[msg.seq].mission_type);
            }else if(msg.mission_type == MAV_MISSION_TYPE_FENCE){
                int reqItem = msg.seq;
                int index = 0;
                int vertexTotal = 0;
                for(int i=0;i<appdataIntGS.numGeofences;++i){
                    vertexTotal += appdataIntGS.fenceVertices[i];
                    if (msg.seq < vertexTotal){
                        if(i > 0){
                            index  = appdataIntGS.fenceVertices[i] - (vertexTotal - reqItem);
                        }
                        else{
                            index  = reqItem;
                        }
                       
                        int _type = appdataIntGS.gfData[i].type;
                        double _ceiling = appdataIntGS.gfData[i].ceiling;
                        double _floor = appdataIntGS.gfData[i].floor;
                        double _numVertices = appdataIntGS.fenceVertices[i];

                        mavlink_msg_mission_item_pack(sysid,compid,&msgMissionItem,target_sys,target_comp,i,
                                                          _type,
                                                          0,
                                                          0,
                                                          0,
                                                        _numVertices,
                                                        index,_floor,_ceiling,
                                                        appdataIntGS.gfData[i].vertices[index][0],
                                                        appdataIntGS.gfData[i].vertices[index][1],
                                                        0,MAV_MISSION_TYPE_FENCE);

                        break;
                    }
                }
            }else if(msg.mission_type == MAV_MISSION_TYPE_RALLY){
                mavlink_msg_mission_item_pack(sysid,compid,&msgMissionItem,target_sys,target_comp,msg.seq,MAV_FRAME_GLOBAL,0,0,0,0,0,0,0,
                                              appdataIntGS.trajectory.waypoints[msg.seq].latitude,
                                              appdataIntGS.trajectory.waypoints[msg.seq].longitude,
                                              appdataIntGS.trajectory.waypoints[msg.seq].altitude,MAV_MISSION_TYPE_RALLY);
            }

            writeMavlinkData(&appdataIntGS.gs,&msgMissionItem);
            break;

        }

        case MAVLINK_MSG_ID_COMMAND_LONG:
        {
            //printf("MAVLINK_MSG_ID_COMMAND_LONG\n");
            mavlink_command_long_t msg;
            mavlink_msg_command_long_decode(&message, &msg);

            if (msg.command == MAV_CMD_MISSION_START) {

                //Publish local array of parameters to SB for other apps
                //printf("Publishing Parameters\n");
                gsInterface_PublishParams();
                //Send confirmation message to GS
                mavlink_message_t paramStatusMsg;
                mavlink_msg_statustext_pack(sysid,compid,&paramStatusMsg,MAV_SEVERITY_INFO,"Parameters Sent to Apps");
                writeMavlinkData(&appdataIntGS.gs,&paramStatusMsg);

                send2ap = false;
                if (appdataIntGS.numWaypoints > 1) {
                    appdataIntGS.startMission.param1 = msg.param1;
                    CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &appdataIntGS.startMission);
                    CFE_SB_SendMsg((CFE_SB_Msg_t *) &appdataIntGS.startMission);

                    mavlink_message_t statusMsg;
                    mavlink_msg_statustext_pack(sysid,compid,&statusMsg,MAV_SEVERITY_INFO,"IC:Starting Mission");
                    writeMavlinkData(&appdataIntGS.gs,&statusMsg);
                }else{
                    mavlink_message_t statusMsg;
                    mavlink_msg_statustext_pack(sysid,compid,&statusMsg,MAV_SEVERITY_WARNING,"IC:No Flight Plan loaded");
                    writeMavlinkData(&appdataIntGS.gs,&statusMsg);

                }
            }
            else if (msg.command == MAV_CMD_DO_FENCE_ENABLE) {
                uint16_t index = msg.param2; 
                appdataIntGS.recvGeofIndex = index;
                if(index >= appdataIntGS.numGeofences){
                    appdataIntGS.numGeofences++;
                }

                appdataIntGS.gfData[index].index = (uint16_t)msg.param2;
                appdataIntGS.gfData[index].type = (uint8_t)msg.param3;
                appdataIntGS.gfData[index].totalvertices = (uint16_t)msg.param4;
                appdataIntGS.gfData[index].floor = msg.param5;
                appdataIntGS.gfData[index].ceiling = msg.param6;

                mavlink_message_t fetchfence;
                mavlink_msg_fence_fetch_point_pack(sysid,compid,&fetchfence,target_sys,target_comp,0);
                writeMavlinkData(&appdataIntGS.gs,&fetchfence);
            }
            else if (msg.command == MAV_CMD_SPATIAL_USER_1) {
                appdataIntGS.traffic.index = (uint32_t)msg.param1;
                appdataIntGS.traffic.type = _TRAFFIC_SIM_;
                appdataIntGS.traffic.latitude = msg.param5;
                appdataIntGS.traffic.longitude = msg.param6;
                appdataIntGS.traffic.altitude = msg.param7;
                appdataIntGS.traffic.vn = msg.param2;
                appdataIntGS.traffic.ve = msg.param3;
                appdataIntGS.traffic.vd = msg.param4;

                CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &appdataIntGS.traffic);
                CFE_SB_SendMsg((CFE_SB_Msg_t *) &appdataIntGS.traffic);
                send2ap = false;
            }else if (msg.command == MAV_CMD_USER_1) {
                noArgsCmd_t resetIcarous;
                CFE_SB_InitMsg(&resetIcarous,ICAROUS_RESET_MID,sizeof(noArgsCmd_t),TRUE);
                SendSBMsg(resetIcarous);
                appdataIntGS.nextWaypointIndex = 1000;
                send2ap = false;

                mavlink_message_t statusMsg;;
                mavlink_msg_statustext_pack(sysid,compid,&statusMsg,MAV_SEVERITY_WARNING,"IC:Resetting Icarous");
                writeMavlinkData(&appdataIntGS.gs,&statusMsg);
            }else if(msg.command == MAV_CMD_USER_2){
                argsCmd_t trackCmd;
                CFE_SB_InitMsg(&trackCmd,ICAROUS_TRACK_STATUS_MID, sizeof(argsCmd_t),TRUE);
                trackCmd.param1 = (float) msg.param1;
                SendSBMsg(trackCmd);
                send2ap = false;
            }else if (msg.command == MAV_CMD_USER_3) {
                argsCmd_t radarCmd;
                CFE_SB_InitMsg(&radarCmd,RADAR_TRIGGER_MID, sizeof(argsCmd_t),TRUE);
                radarCmd.name = _RADAR_;
                CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &radarCmd);
                CFE_SB_SendMsg((CFE_SB_Msg_t *) &radarCmd);
                send2ap = false;
                radarCmd.param1 =  msg.param1;
                OS_printf("Received radar command %f\n",msg.param1);
                SendSBMsg(radarCmd);
                mavlink_message_t status;
                mavlink_msg_statustext_pack(sysid,compid,&status,MAV_SEVERITY_INFO,"Received radar command");
                SendGSMsg(status);

            }else if (msg.command == MAV_CMD_USER_5) {
                noArgsCmd_t ditchCmd;
                CFE_SB_InitMsg(&ditchCmd,ICAROUS_DITCH_MID, sizeof(noArgsCmd_t),TRUE);
                ditchCmd.name = _DITCH_;
                CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &ditchCmd);
                CFE_SB_SendMsg((CFE_SB_Msg_t *) &ditchCmd);
                send2ap = false;
                OS_printf("Received ditching command \n");

                mavlink_message_t status;
                mavlink_msg_statustext_pack(sysid,compid,&status,MAV_SEVERITY_INFO,"Received ditching command");
                SendGSMsg(status);
            }
            else {
                //writeMavlinkData(&appdataInt.ap,&message);
            }

            break;
        }

        case MAVLINK_MSG_ID_FENCE_POINT:
        {
            //printf("MAVLINK_MSG_ID_FENCE_POINT\n");
            uint16_t index = appdataIntGS.recvGeofIndex;

            mavlink_fence_point_t msg;
            mavlink_msg_fence_point_decode(&message,&msg);
            int count = msg.idx;
            int total = msg.count;

            appdataIntGS.fenceVertices[index] = total;

            appdataIntGS.gfData[index].vertices[msg.idx][0] = msg.lat;
            appdataIntGS.gfData[index].vertices[msg.idx][1] = msg.lng;

            if (count < total-1) {
                mavlink_message_t fetchfence;
                mavlink_msg_fence_fetch_point_pack(sysid,compid,&fetchfence,target_sys,target_comp,count+1);
                writeMavlinkData(&appdataIntGS.gs,&fetchfence);
            }else{
                mavlink_message_t ack;
                mavlink_msg_command_ack_pack(sysid,compid,&ack,MAV_CMD_DO_FENCE_ENABLE,MAV_RESULT_ACCEPTED);
                writeMavlinkData(&appdataIntGS.gs,&ack);
            }

            if(count == total - 1) {
                geofence_t data2send;

                memcpy(&data2send,appdataIntGS.gfData + index,sizeof(geofence_t));

                CFE_SB_InitMsg(&data2send,ICAROUS_GEOFENCE_MID,sizeof(geofence_t),FALSE);
                SendSBMsg(data2send);	
            }

            break;
        }

        case MAVLINK_MSG_ID_PARAM_SET:
        {
            //printf("MAVLINK_MSG_ID_PARAM_SET\n");
            mavlink_param_set_t msg;
            mavlink_msg_param_set_decode(&message, &msg);

            //Store value of the parameter locally and send confirmation back to GS
            //Determine which index to save the parameter to
            for (int i = 0; i < PARAM_COUNT; i++) {
                if (strcmp(msg.param_id, appdataIntGS.param_ids[i])==0) {
                    //Create a mavlink param_value message for the parameter being set
                    mavlink_param_value_t paramValue;
                    paramValue.param_count = PARAM_COUNT;
                    strcpy(paramValue.param_id,msg.param_id);
                    paramValue.param_index = i;
                    paramValue.param_type = msg.param_type;
                    paramValue.param_value = msg.param_value;

                    //Store the param_value message at the param_index, i
                    appdataIntGS.params[i] = paramValue;

                    //Send a param_value message back to the GS as confirmation
                    mavlink_message_t param_value_msg;
                    mavlink_msg_param_value_pack(sysid, compid, &param_value_msg,
                                                 msg.param_id,
                                                 msg.param_value,
                                                 msg.param_type,
                                                 PARAM_COUNT, i);
                    SendGSMsg(param_value_msg);


                    if(paramValue.param_index == PARAM_COUNT-1){
                        mavlink_message_t status;
                        mavlink_msg_statustext_pack(sysid,compid,&status,MAV_SEVERITY_INFO,"Received all parameters");
                        SendGSMsg(status);
                    }

                    break;
                }
            }

            break;
        }

        case MAVLINK_MSG_ID_PARAM_REQUEST_LIST:
        {
            //printf("MAVLINK_MSG_ID_REQUEST_LIST\n");
            mavlink_param_request_list_t msg;
            mavlink_msg_param_request_list_decode(&message, &msg);

            //Send all locally stored parameters to the GS as mavlink PARAM_VALUE messages
            for (int i = 0; i < PARAM_COUNT; i++) {
                //SendGSMsg(appdataIntGS.params[i]);
                mavlink_message_t param_value_msg;
                mavlink_msg_param_value_pack(sysid, compid, &param_value_msg,
                                                 appdataIntGS.param_ids[i],
                                                 appdataIntGS.params[i].param_value,
                                                 appdataIntGS.params[i].param_type,
                                                 PARAM_COUNT, i);

                writeMavlinkData(&appdataIntGS.gs,&param_value_msg);
                //printf("Sending parameter : %s: %f\n",appdataIntGS.param_ids[i],appdataIntGS.params[i].param_value);
            }

            break;
        }

        case MAVLINK_MSG_ID_PARAM_REQUEST_READ:
        {
            //printf("MAVLINK_MSG_ID_REQUEST_READ\n");
            mavlink_param_request_read_t msg;
            mavlink_msg_param_request_read_decode(&message, &msg);

            //Send requested parameter as a mavlink PARAM_VALUE message
            //(Assume valid param_index is given, so no need to look up params by param_id)
            mavlink_message_t param_value_msg;
            mavlink_msg_param_value_pack(sysid, compid, &param_value_msg,
                                         appdataIntGS.param_ids[msg.param_index],
                                         appdataIntGS.params[msg.param_index].param_value,
                                         appdataIntGS.params[msg.param_index].param_type,
                                         PARAM_COUNT, msg.param_index);
            SendGSMsg(param_value_msg);
            //printf("Requested param id : %s\n",msg.param_id);
            //printf("Requested param index : %d\n",msg.param_index);
            //printf("Sending parameter : %s: %f\n",appdataIntGS.param_ids[msg.param_index],appdataIntGS.params[msg.param_index].param_value);

            break;
        }
    }
}

void gsInterface_ProcessPacket() {
    CFE_SB_MsgId_t  MsgId;

    argsCmd_t* cmd;

    MsgId = CFE_SB_GetMsgId(appdataIntGS.INTERFACEMsgPtr);
    switch (MsgId)
    {

        case ICAROUS_BANDS_TRACK_MID:
        {
            mavlink_message_t msg;
            bands_t* bands = (bands_t*) appdataIntGS.INTERFACEMsgPtr;
            mavlink_msg_icarous_kinematic_bands_pack(sysid,compid,&msg,(int8_t)bands->numBands,
                    (uint8_t)bands->type[0],(float)bands->min[0],(float)bands->max[0],
                    (uint8_t)bands->type[1],(float)bands->min[1],(float)bands->max[1],
                    (uint8_t)bands->type[2],(float)bands->min[2],(float)bands->max[2],
                    (uint8_t)bands->type[3],(float)bands->min[3],(float)bands->max[3],
                    (uint8_t)bands->type[4],(float)bands->min[4],(float)bands->max[4]);


            if(bands->numBands > 0) {
                writeMavlinkData(&appdataIntGS.gs, &msg);
            }
            break;
        }

        case DOWNLINK_FLIGHTPLAN_MID:{
            flightplan_t *msg = (flightplan_t*) appdataIntGS.INTERFACEMsgPtr;
            memcpy(&appdataIntGS.fpData, msg,sizeof(flightplan_t));
            break;
        }

        case ICAROUS_STATUS_MID:{
            status_t* statusMsg = (status_t*) appdataIntGS.INTERFACEMsgPtr;
            mavlink_message_t msg;
            mavlink_msg_statustext_pack(sysid,compid,&msg,statusMsg->severity,statusMsg->buffer);
            writeMavlinkData(&appdataIntGS.gs,&msg);
            break;
        }

        case ICAROUS_TRAJECTORY_MID:{
            flightplan_t* fp = (flightplan_t*) appdataIntGS.INTERFACEMsgPtr;
            memcpy(&appdataIntGS.trajectory, fp, sizeof(flightplan_t));

            mavlink_message_t msg;
            mavlink_msg_mission_count_pack(sysid,compid,&msg,target_sys,target_comp,fp->num_waypoints,MAV_MISSION_TYPE_RALLY);
            writeMavlinkData(&appdataIntGS.gs,&msg);
            break;
        }

        case ICAROUS_POSITION_MID:{
            position_t* pos = (position_t*) appdataIntGS.INTERFACEMsgPtr;

            if(pos->aircraft_id == CFE_PSP_GetSpacecraftId()) {
                mavlink_message_t msg;
                mavlink_msg_global_position_int_pack(sysid, compid, &msg, (uint32_t) pos->time_gps,
                                                     (int32_t) (pos->latitude * 1E7),
                                                     (int32_t) (pos->longitude * 1E7),
                                                     (int32_t) (pos->altitude_abs * 1E3),
                                                     (int32_t) (pos->altitude_rel * 1E3),
                                                     (int16_t) (pos->vn * 1E2),
                                                     (int16_t) (pos->ve * 1E2),
                                                     (int16_t) (pos->vd * 1E2),
                                                     (uint16_t) (pos->hdg * 1E2));

                writeMavlinkData(&appdataIntGS.gs, &msg);
            }else{
                mavlink_message_t msg;

                double heading = fmod(2*M_PI + atan2(pos->ve,pos->vn),2*M_PI)*180/M_PI;
                double speed = sqrt(pos->vn*pos->vn + pos->ve*pos->ve);
                mavlink_msg_adsb_vehicle_pack(sysid,compid,&msg,(uint8_t)pos->aircraft_id,
                                          (int32_t)(pos->latitude*1E7),
                                          (int32_t)(pos->longitude*1E7),
                                          ADSB_ALTITUDE_TYPE_GEOMETRIC,
                                          (int32_t)(pos->altitude_rel*1E3),
                                          (uint16_t) (heading*1E2),
                                          (uint16_t) (speed*1E2),
                                          (uint16_t) (pos->vd*1E2),
                                          "NONE",0,0,0,0);

                writeMavlinkData(&appdataIntGS.gs,&msg);

            }
            break;

        }

        case ICAROUS_VFRHUD_MID:{
            vfrhud_t* vfrhud = (vfrhud_t*) appdataIntGS.INTERFACEMsgPtr;
            appdataIntGS.currentApMode = vfrhud->modeAP;
            appdataIntGS.currentIcarousMode = vfrhud->modeIcarous;

            mavlink_message_t msg1;
            mavlink_msg_vfr_hud_pack(sysid,compid,&msg1,(float)vfrhud->airspeed,
                                     (float)vfrhud->groundspeed,
                                     vfrhud->heading,
                                     vfrhud->throttle,
                                     (float)vfrhud->alt,
                                     (float)vfrhud->climb);
            writeMavlinkData(&appdataIntGS.gs,&msg1);

            mavlink_message_t msg2;
            mavlink_msg_mission_current_pack(sysid,compid,&msg2,vfrhud->waypointCurrent);
            writeMavlinkData(&appdataIntGS.gs,&msg2);

            break;
        }

        case ICAROUS_TRAFFIC_MID:{
            object_t *traffic = (object_t *)appdataIntGS.INTERFACEMsgPtr;
            mavlink_message_t msg;

            uint8_t emitterType = 0;
            if (traffic->type == _TRAFFIC_RADAR_)
            {
                emitterType = 100;
            }else if(traffic->type == _TRAFFIC_SIM_){
                emitterType = 255;
            }

            double heading = fmod(2 * M_PI + atan2(traffic->ve, traffic->vn), 2 * M_PI) * 180 / M_PI;
            double speed = sqrt(traffic->vn * traffic->vn + traffic->ve * traffic->ve);
            mavlink_msg_adsb_vehicle_pack(sysid, compid, &msg, (uint8_t)traffic->index,
                                          (int32_t)(traffic->latitude * 1E7),
                                          (int32_t)(traffic->longitude * 1E7),
                                          ADSB_ALTITUDE_TYPE_GEOMETRIC,
                                          (int32_t)(traffic->altitude * 1E3),
                                          (uint16_t)(heading * 1E2),
                                          (uint16_t)(speed * 1E2),
                                          (uint16_t)(traffic->vd * 1E2),
                                          "NONE", emitterType, 0, 0, 0);

            writeMavlinkData(&appdataIntGS.gs, &msg);
            break;
        }

        case ICAROUS_ATTITUDE_MID: {
            attitude_t* attitude = (attitude_t*) appdataIntGS.INTERFACEMsgPtr;


            double roll = (attitude->roll > 180)? -(180-(attitude->roll - 180)):attitude->roll;
            double pitch = (attitude->pitch > 180)? -(180-(attitude->pitch - 180)):attitude->pitch;
            double yaw = (attitude->yaw > 180)? -(180-(attitude->yaw - 180)):attitude->yaw;
            double rollspeed = (attitude->rollspeed > 180)? -(180-(attitude->rollspeed - 180)):attitude->rollspeed;
            double pitchspeed = (attitude->pitchspeed > 180)? -(180-(attitude->pitchspeed - 180)):attitude->pitchspeed;
            double yawspeed = (attitude->yawspeed > 180)? -(180-(attitude->yawspeed - 180)):attitude->yawspeed;

            mavlink_message_t msg;

            mavlink_msg_attitude_pack(sysid,compid,&msg,
                                      (uint32) attitude->time_boot*M_PI/180,
                                      (float) roll*M_PI/180,
                                      (float) pitch*M_PI/180,
                                      (float) yaw*M_PI/180,
                                      (float) rollspeed*M_PI/180,
                                      (float) pitchspeed*M_PI/180,
                                      (float) yawspeed*M_PI/180);

            //printf("IC sending attitude info to GCS\n");
            writeMavlinkData(&appdataIntGS.gs,&msg);
            break;
        }

        case ICAROUS_BATTERY_STATUS_MID: {
            battery_status_t* battery_status = (battery_status_t*) appdataIntGS.INTERFACEMsgPtr;

            mavlink_message_t msg;

            mavlink_msg_battery_status_pack(sysid,compid,&msg,
                                            battery_status->id,
                                            battery_status->battery_function,
                                            battery_status->type,
                                            battery_status->temperature,
                                            battery_status->voltages,
                                            battery_status->current_battery,
                                            battery_status->current_consumed,
                                            battery_status->energy_consumed,
                                            battery_status->battery_remaining);

            //printf("IC sending battery info to GCS\n");
            writeMavlinkData(&appdataIntGS.gs,&msg);
            break;
        }
    }
}

void ConvertMissionItemsToPlan(uint16_t  size, mavlink_mission_item_t items[],flightplan_t* fp){

    int count = 0;
    for(int i=0;i<size;++i){
        switch(items[i].command){

            case MAV_CMD_NAV_WAYPOINT: {
                fp->waypoints[count].latitude = items[i].x;
                fp->waypoints[count].longitude = items[i].y;
                fp->waypoints[count].altitude = items[i].z;
                fp->waypoints[count].wp_metric = WP_METRIC_NONE;
                count++;
                //OS_printf("constructed waypoint\n");
                break;
            }

            case MAV_CMD_NAV_SPLINE_WAYPOINT:{
                fp->waypoints[count].latitude = items[i].x;
                fp->waypoints[count].longitude = items[i].y;
                fp->waypoints[count].altitude = items[i].z;
                fp->waypoints[count].wp_metric = WP_METRIC_NONE;
                count++;
                break;
            }

            case MAV_CMD_NAV_LOITER_TIME:{
                fp->waypoints[count].latitude = items[i].x;
                fp->waypoints[count].longitude = items[i].y;
                fp->waypoints[count].altitude = items[i].z;
                if(items[i].command == MAV_CMD_NAV_LOITER_TIME){
                    fp->waypoints[count].wp_metric = WP_METRIC_ETA;
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
                    fp->waypoints[count-1].wp_metric = WP_METRIC_ETA;
                    //OS_printf("Setting ETA to %f\n",eta);
                }
                break;
            }
        }
    }

    fp->num_waypoints = count;
}

