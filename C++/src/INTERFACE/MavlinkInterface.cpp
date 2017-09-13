#include <Port.h>
#include <Icarous_msg.h>
#include <iostream>
#include "MAVLinkInterface.h"
#include "Icarous.h"

MAVLinkInterface_t::MAVLinkInterface_t(Icarous_t* ic):Interface_t(ic){
    waypointType = NULL;
    numWaypoints = 0;
}

ArduPilotInterface_t::ArduPilotInterface_t(Icarous_t *ic):MAVLinkInterface_t(ic) {
    foundVehicle = false;
    time(&lastPing);
    time(&newPing);
    elapsedTime = 0;
}

MAVProxyInterface_t::MAVProxyInterface_t(Icarous_t *ic):MAVLinkInterface_t(ic) {
    speed = 1;
}

MAVLinkInterface_t::~MAVLinkInterface_t() {
    delete PT;
    delete waypointType;
}

void MAVLinkInterface_t::ConfigurePorts(char *portname, int baudrate) {
    PT = (Port_t*)new SerialPort_t(portname,baudrate,0);
}

void MAVLinkInterface_t::ConfigurePorts(char *hostname, int input, int output) {
    PT = (Port_t*) new SocketPort_t(hostname,input,output);
}

void MAVLinkInterface_t::GetData() {
    mavlink_message_t message;
    mavlink_status_t status;
    while(true) {
        int n = PT->ReadData();
        uint8_t msgReceived = 0;
        for (int i = 0; i < n; i++) {
            uint8_t cp = PT->recvbuffer[i];
            msgReceived = mavlink_parse_char(MAVLINK_COMM_0, cp, &message, &status);
            if (msgReceived) {
                // Send SB message if necessary
                ProcessMessage(&message);
            }
        }
    }
}


void MAVLinkInterface_t::SendData(msgType _type, void *data) {
    switch(_type){
        case MSG_ID_CMD:
            SendCommands((ArgsCmd_t*)data);
            break;
        case MSG_ID_BANDS:
            SendBands((visbands_t*)data);
            break;
        default:
            break;
    }
}

void MAVLinkInterface_t::SendMessage(const mavlink_message_t* message) {
    uint8_t sendbuffer[300];
    uint16_t len = mavlink_msg_to_send_buffer(sendbuffer, message);
    PT->WriteData(sendbuffer,len);
}



void MAVLinkInterface_t::ProcessMessage(mavlink_message_t* message) {
    switch(message->msgid){
        case MAVLINK_MSG_ID_GLOBAL_POSITION_INT:
        {
            HandlePosition(message);
            break;
        }

        case MAVLINK_MSG_ID_COMMAND_ACK:
        {
            HandleCommandAck(message);
            break;
        }

        case MAVLINK_MSG_ID_MISSION_ITEM_REACHED:
        {
            HandleMissionItemReached(message);
            break;
        }

        case MAVLINK_MSG_ID_HEARTBEAT:
        {
            HandleHeartbeat(message);
            break;
        }

        case MAVLINK_MSG_ID_MISSION_COUNT:
        {
            HandleMissionCount(message);
            break;
        }

        case MAVLINK_MSG_ID_MISSION_ITEM:
        {
            //printf("MAVLINK_MSG_ID_MISSION_ITEM\n");
            HandleMissionItem(message);
            break;
        }


        case MAVLINK_MSG_ID_MISSION_REQUEST_LIST:
        {
            //printf("MAVLINK_MSG_ID_MISSION_REQUEST_LIST\n");
            HandleMissionRequestList(message);
            break;
        }

        case MAVLINK_MSG_ID_MISSION_REQUEST:
        {
            //printf("MAVLINK_MSG_ID_MISSION_REQUEST\n");
            HandleMissionRequest(message);
            break;
        }


        case MAVLINK_MSG_ID_PARAM_REQUEST_LIST:
        {
            //printf("MAVLINK_MSG_ID_PARAM_REQUEST_LIST\n");
            HandleParamRequestList(message);
            break;
        }

        case MAVLINK_MSG_ID_PARAM_REQUEST_READ:
        {
            //printf("MAVLINK_MSG_ID_PARAM_REQUEST_READ\n");
            HandleParamRequestRead(message);
            break;
        }

        case MAVLINK_MSG_ID_PARAM_VALUE:
        {
            //printf("MAVLINK_MSG_ID_PARAM_SET\n");
            HandleParamValue(message);
            break;
        }

        case MAVLINK_MSG_ID_PARAM_SET:
        {
            //printf("MAVLINK_MSG_ID_PARAM_SET\n");
            HandleParamSet(message);
            break;
        }

        case MAVLINK_MSG_ID_COMMAND_LONG:
        {
            HandleCommandLong(message);
            break;
        }

        case MAVLINK_MSG_ID_COMMAND_INT:
        {
            //printf("MAVLINK_MSG_ID_COMMAND_INT\n");
            HandleCommandInt(message);
            break;
        }

        case MAVLINK_MSG_ID_SET_MODE:
        {
            //printf("MAVLINK_MSG_ID_SET_MODE\n");
            HandleSetMode(message);
            break;
        }


        case MAVLINK_MSG_ID_FENCE_POINT:
        {
            //printf("MAVLINK_MSG_ID_FENCE_POINT\n");
            HandleFencePoint(message);
            break;
        }

        case MAVLINK_MSG_ID_RADIO:
        {
            HandleRadio(message);
            break;
        }

        case MAVLINK_MSG_ID_RADIO_STATUS:
        {
            HandleRadioStatus(message);
            break;
        }
    }
}

void ArduPilotInterface_t::GetData() {
    mavlink_message_t message;
    mavlink_status_t status;
    while(true) {
        int n = PT->ReadData();
        uint8_t msgReceived = 0;
        for (int i = 0; i < n; i++) {
            uint8_t cp = PT->recvbuffer[i];
            msgReceived = mavlink_parse_char(MAVLINK_COMM_0, cp, &message, &status);
            if (msgReceived) {

                // Send data to pipe
                pipe->SendMessage(&message);

                // Send SB message if necessary
                ProcessMessage(&message);
            }
        }
    }
}

void ArduPilotInterface_t::HandleHeartbeat(const mavlink_message_t *message) {
    mavlink_heartbeat_t msg;
    mavlink_msg_heartbeat_decode(message,&msg);

    lastPing = newPing;
    time(&newPing);
    elapsedTime = difftime(newPing, lastPing);

    if(elapsedTime > 7){
        foundVehicle = false;
        std::cout << "Reconnecting with vehicle" << std::endl;
    }

    if(!foundVehicle){
        mavlink_message_t msgReqStream;
        mavlink_msg_request_data_stream_pack(255,0,&msgReqStream,1,0,MAV_DATA_STREAM_ALL,5,1);
        foundVehicle = true;
    }
}

void ArduPilotInterface_t::HandleMissionItemReached(const mavlink_message_t *message){
    mavlink_mission_item_reached_t msg;
    mavlink_msg_mission_item_reached_decode(message, &msg);


    if(pipe->waypointType[(int)msg.seq] == MAV_CMD_NAV_WAYPOINT ||
       pipe->waypointType[(int)msg.seq] == MAV_CMD_NAV_SPLINE_WAYPOINT){
        missionItemReached_t wpreached;
        wpreached.reachedwaypoint = msg.seq;
        // Send wpreached to Icarous
        icarous->InputMissionItemReached(&wpreached);
    }

}

void ArduPilotInterface_t::HandleCommandAck(const mavlink_message_t *message){
    mavlink_command_ack_t msg;
    mavlink_msg_command_ack_decode(message, &msg);
    uint8_t send = 0;
    CmdAck_t ack;
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
       // Input ack to Icarous
        icarous->InputAck(&ack);
    }
}

void ArduPilotInterface_t::HandlePosition(const mavlink_message_t *message){
    mavlink_global_position_int_t globalPositionInt;
    mavlink_msg_global_position_int_decode(message,&globalPositionInt);

    position_t position;
    position.time_gps  = (double)globalPositionInt.time_boot_ms/1E3;
    position.latitude  = (double)globalPositionInt.lat/1E7;
    position.longitude = (double)globalPositionInt.lon/1E7;
    position.altitude_abs  = (double)globalPositionInt.alt/1E3;
    position.altitude_rel  = (double)globalPositionInt.relative_alt/1E3;
    position.vx = (double)globalPositionInt.vx/100;
    position.vy = (double)globalPositionInt.vy/100;
    position.vz = (double)globalPositionInt.vz/100;

    icarous->InputPosition(&position);
}

void ArduPilotInterface_t::SendCommands(ArgsCmd_t * cmd) {

    mavlink_message_t msg;
    control_mode_t mode;
    switch(cmd->name){

        case _ARM_:
        {
            mavlink_msg_command_long_pack(255,0,&msg,1,0,MAV_CMD_COMPONENT_ARM_DISARM,0,cmd->param1,0,0,0,0,0,0);
            SendMessage(&msg);
            break;
        }

        case _TAKEOFF_:
        {
            mavlink_msg_command_long_pack(255,0,&msg,1,0,MAV_CMD_NAV_TAKEOFF,0,1,0,0,0,0,0,cmd->param1);
            SendMessage(&msg);
            break;
        }

        case _SETMODE_:
        {
            if(cmd->param1 == _PASSIVE_){
                mode = AUTO;
            }else if(cmd->param1 == _ACTIVE_){
                mode = GUIDED;
            }


            mavlink_msg_set_mode_pack(255,0,&msg,1,1,mode);
            SendMessage(&msg);
            break;
        }

        case _LAND_:
        {
            mavlink_msg_command_long_pack(255,0,&msg,1,0,MAV_CMD_NAV_LAND,0,0,0,0,0,0,0,0);
            SendMessage(&msg);
            break;
        }

        case _GOTOWP_:
        {
            int tempSeq = (int)cmd->param1;
            int seq = -1;
            for(int i=0;i<=tempSeq;i++){
                seq++;
                int val = (pipe->waypointType[seq] == MAV_CMD_NAV_WAYPOINT) ||
                          (pipe->waypointType[seq] == MAV_CMD_NAV_SPLINE_WAYPOINT);
                if (!val){
                    i = i-1 ;
                }
            }
            mavlink_msg_mission_set_current_pack(255,0,&msg,1,0,seq);
            SendMessage(&msg);
            break;
        }

        case _SETPOS_:
        {
            mavlink_msg_set_position_target_global_int_pack(255,0,&msg,0,1,0,MAV_FRAME_GLOBAL_RELATIVE_ALT_INT,
                                                            0b0000111111111000,(int)(cmd->param1*1E7),(int)(cmd->param2*1E7),(cmd->param3),
                                                            0,0,0,0,0,0,0,0);
            SendMessage(&msg);
            break;
        }

        case _SETVEL_:
        {
            mavlink_msg_set_position_target_local_ned_pack(255,0,&msg,0,1,0,MAV_FRAME_LOCAL_NED, 0b0000111111000111,0,0,0,
                                                           (float)cmd->param1,(float)cmd->param2,(float)cmd->param3,
                                                           0,0,0,0,0);
            SendMessage(&msg);
            break;
        }

        case _SETYAW_:
        {
            mavlink_msg_command_long_pack(255,0,&msg,1,0,MAV_CMD_CONDITION_YAW,0,
                                          (float)cmd->param1,(float)cmd->param2,(float)cmd->param3,(float)cmd->param4,0,0,0);
            SendMessage(&msg);
            break;
        }

        case _SETSPEED_:
        {
            mavlink_msg_command_long_pack(255,0,&msg,1,0,MAV_CMD_DO_CHANGE_SPEED,0,
                                          1,(float)cmd->param1,0,0,0,0,0);
            SendMessage(&msg);
            break;
        }

    }
}

void MAVProxyInterface_t::HandleMissionRequestList(const mavlink_message_t *message) {
    mavlink_mission_request_list_t msg;
    mavlink_msg_mission_request_list_decode(message, &msg);
    pipe->SendMessage(message);
}

void MAVProxyInterface_t::HandleFencePoint(const mavlink_message_t *message) {
    mavlink_fence_point_t msg;
    mavlink_msg_fence_point_decode(message,&msg);
    int count = msg.idx;
    int total = msg.count;

    gfdata.vertexIndex = msg.idx;
    gfdata.latitude = msg.lat;
    gfdata.longitude = msg.lng;

    // Send GF data to Icarous
    icarous->InputGeofenceData(&gfdata);
    if(count < total-1){
        mavlink_message_t fetchfence;
        mavlink_msg_fence_fetch_point_pack(1,1,&fetchfence,255,0,count+1);
        SendMessage(&fetchfence);
    }else{
        mavlink_message_t ack;
        mavlink_msg_command_ack_pack(1,0,&ack,MAV_CMD_DO_FENCE_ENABLE,1);
        SendMessage(&ack);
    }
}

void MAVProxyInterface_t::HandleReset(const mavlink_command_long_t &msg){
    icarous->InputResetIcarous();
}

void MAVProxyInterface_t::HandleTraffic(const mavlink_command_long_t &msg) {
    object_t traffic;
    traffic.type  = 1;
    traffic.index = msg.param1;
    traffic.latitude = msg.param5;
    traffic.longitude = msg.param6;
    traffic.altiude = msg.param7;
    traffic.vx = msg.param2;
    traffic.vy = msg.param3;
    traffic.vz = msg.param4;

    // Send to Icarous
    icarous->InputTraffic(&traffic);
}

void MAVProxyInterface_t::HandleFenceEnable(const mavlink_command_long_t &msg){
    gfdata.index = msg.param2;
    gfdata.type = msg.param3;
    gfdata.totalvertices = msg.param4;
    gfdata.floor = msg.param5;
    gfdata.ceiling = msg.param6;


    mavlink_message_t fetchfence;
    mavlink_msg_fence_fetch_point_pack(1,1,&fetchfence,255,0,0);

    SendMessage(&fetchfence);
}

void MAVProxyInterface_t::HandleMissionStart(const mavlink_command_long_t &msg){
   // Call start mission in Icarous
    icarous->InputStartMission(msg.param1);
}

void MAVProxyInterface_t::HandleMissionItem(const mavlink_message_t *message) {
    mavlink_mission_item_t msg;
    mavlink_msg_mission_item_decode(message, &msg);
    pipe->SendMessage(message);

    waypointType[(int)msg.seq] = msg.command;
    waypoint_t wpdata;
    if(msg.command == MAV_CMD_NAV_WAYPOINT || msg.command == MAV_CMD_NAV_SPLINE_WAYPOINT){
        // Send message to SB
        wpdata.totalWayPoints = numWaypoints;
        wpdata.wayPointIndex  = msg.seq;
        wpdata.speed     = speed;
        wpdata.latitude  = msg.x;
        wpdata.longitude = msg.y;
        wpdata.altitude  = msg.z;

        // Send Icarous the waypoint
        icarous->InputFlightPlanData(&wpdata);
    }else if(msg.command == MAV_CMD_DO_CHANGE_SPEED){
        speed = msg.param2;
    }
}

void MAVProxyInterface_t::HandleMissionCount(const mavlink_message_t *message) {//printf("MAVLINK_MSG_ID_MISSION_COUNT\n");
    mavlink_mission_count_t msg;
    mavlink_msg_mission_count_decode(message, &msg);
    pipe->SendMessage(message);
    numWaypoints = msg.count;
    speed = 2.0;
    delete waypointType;
    waypointType = new int[msg.count];

    // Clear the flight plan in icarous
    icarous->InputClearFlightPlan();
}



void MAVProxyInterface_t::SendBands(visbands_t *bands) {
    mavlink_message_t msg;
    mavlink_msg_kinematic_bands_pack(1,0,&msg,bands->numBands,
                                     bands->type1,bands->min1,bands->max1,
                                     bands->type2,bands->min2,bands->max2,
                                     bands->type3,bands->min3,bands->max3,
                                     bands->type4,bands->min4,bands->max4,
                                     bands->type5,bands->min5,bands->max5);

    SendMessage(&msg);
}

void MAVProxyInterface_t::HandleMissionRequest(const mavlink_message_t *message) {
    mavlink_mission_request_t msg;
    mavlink_msg_mission_request_decode(message, &msg);
    pipe->SendMessage(message);
}

void MAVProxyInterface_t::HandleParamRequestList(const mavlink_message_t *message) {
    //printf("MAVLINK_MSG_ID_PARAM_REQUEST_LIST\n");
    mavlink_param_request_list_t msg;
    mavlink_msg_param_request_list_decode(message, &msg);
    pipe->SendMessage(message);
}

void MAVProxyInterface_t::HandleParamRequestRead(const mavlink_message_t *message) {
    mavlink_param_request_read_t msg;
    mavlink_msg_param_request_read_decode(message, &msg);
    pipe->SendMessage(message);
}

void MAVProxyInterface_t::HandleParamValue(const mavlink_message_t *message) {
    mavlink_param_value_t msg;
    mavlink_msg_param_value_decode(message, &msg);
    pipe->SendMessage(message);
}

void MAVProxyInterface_t::HandleParamSet(const mavlink_message_t *message) {
    mavlink_param_set_t msg;
    mavlink_msg_param_set_decode(message, &msg);
    pipe->SendMessage(message);
}

void MAVProxyInterface_t::HandleCommandLong(const mavlink_message_t *message) {
    mavlink_command_long_t msg;
    mavlink_msg_command_long_decode(message, &msg);

    if(msg.command == MAV_CMD_MISSION_START){
        HandleMissionStart(msg);
    }
    else if(msg.command == MAV_CMD_DO_FENCE_ENABLE){
        HandleFenceEnable(msg);
    }
    else if(msg.command == MAV_CMD_SPATIAL_USER_1){
        HandleTraffic(msg);
    }
    else if(msg.command == MAV_CMD_USER_1){
        // Call reset function in Icarous
        HandleReset(msg);
    }
    else {
        pipe->SendMessage(message);
    }

}

void MAVProxyInterface_t::HandleCommandInt(const mavlink_message_t *message) {
    mavlink_command_int_t msg;
    mavlink_msg_command_int_decode(message, &msg);
    pipe->SendMessage(message);
}

void MAVProxyInterface_t::HandleSetMode(const mavlink_message_t *message) {
    mavlink_set_mode_t msg;
    mavlink_msg_set_mode_decode(message, &msg);
    pipe->SendMessage(message);
}

void MAVProxyInterface_t::HandleRadio(const mavlink_message_t *message) {
    pipe->SendMessage(message);
}

void MAVProxyInterface_t::HandleRadioStatus(const mavlink_message_t *message) {
    pipe->SendMessage(message);
}