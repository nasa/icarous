/**
 * MAVLink message decoder
 * 
 * This is a class that will decode MAVLink messages
 *
 * Contact: Swee Balachandran (swee.balachandran@nianet.org)
 * 
 * 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 *
 * Notices:
 *  Copyright 2016 United States Government as represented by the Administrator of the National Aeronautics and Space Administration. 
 *  All rights reserved.
 *     
 * Disclaimers:
 *  No Warranty: THE SUBJECT SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY OF ANY KIND, EITHER EXPRESSED, 
 *  IMPLIED, OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, ANY WARRANTY THAT THE SUBJECT SOFTWARE WILL CONFORM TO SPECIFICATIONS, ANY
 *  IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, OR FREEDOM FROM INFRINGEMENT, 
 *  ANY WARRANTY THAT THE SUBJECT SOFTWARE WILL BE ERROR FREE, OR ANY WARRANTY THAT DOCUMENTATION, IF PROVIDED, 
 *  WILL CONFORM TO THE SUBJECT SOFTWARE. THIS AGREEMENT DOES NOT, IN ANY MANNER, CONSTITUTE AN ENDORSEMENT BY GOVERNMENT 
 *  AGENCY OR ANY PRIOR RECIPIENT OF ANY RESULTS, RESULTING DESIGNS, HARDWARE, SOFTWARE PRODUCTS OR ANY OTHER APPLICATIONS 
 *  RESULTING FROM USE OF THE SUBJECT SOFTWARE.  FURTHER, GOVERNMENT AGENCY DISCLAIMS ALL WARRANTIES AND 
 *  LIABILITIES REGARDING THIRD-PARTY SOFTWARE, IF PRESENT IN THE ORIGINAL SOFTWARE, AND DISTRIBUTES IT "AS IS."
 *
 * Waiver and Indemnity:  
 *   RECIPIENT AGREES TO WAIVE ANY AND ALL CLAIMS AGAINST THE UNITED STATES GOVERNMENT, 
 *   ITS CONTRACTORS AND SUBCONTRACTORS, AS WELL AS ANY PRIOR RECIPIENT.  IF RECIPIENT'S USE OF THE SUBJECT SOFTWARE 
 *   RESULTS IN ANY LIABILITIES, DEMANDS, DAMAGES,
 *   EXPENSES OR LOSSES ARISING FROM SUCH USE, INCLUDING ANY DAMAGES FROM PRODUCTS BASED ON, OR RESULTING FROM, 
 *   RECIPIENT'S USE OF THE SUBJECT SOFTWARE, RECIPIENT SHALL INDEMNIFY AND HOLD HARMLESS THE UNITED STATES GOVERNMENT, 
 *   ITS CONTRACTORS AND SUBCONTRACTORS, AS WELL AS ANY PRIOR RECIPIENT, TO THE EXTENT PERMITTED BY LAW.  
 *   RECIPIENT'S SOLE REMEDY FOR ANY SUCH MATTER SHALL BE THE IMMEDIATE, UNILATERAL TERMINATION OF THIS AGREEMENT.
 */

 #include "MAVLinkMessages.h"

 MAVLinkMessages_t::MAVLinkMessages_t(){
     pthread_mutex_init(&lock, NULL);
     memset(&globalPositionInt,0,sizeof(mavlink_global_position_int_t));
 }

 void MAVLinkMessages_t::DecodeMessage(mavlink_message_t message){
    pthread_mutex_lock(&lock);

    switch (message.msgid){

        case MAVLINK_MSG_ID_HEARTBEAT:
        {
            //printf("MAVLINK_MSG_ID_HEARTBEAT\n");
            mavlink_msg_heartbeat_decode(&message, &heartbeat);
            break;
        }

        
        case MAVLINK_MSG_ID_MISSION_COUNT:
        {
            //printf("MAVLINK_MSG_ID_MISSION_COUNT\n");
            mavlink_mission_count_t msg;
            mavlink_msg_mission_count_decode(&message, &msg);
            listMissionCount.push(msg);
            break;
        }

        case MAVLINK_MSG_ID_MISSION_ITEM:
        {
            //printf("MAVLINK_MSG_ID_MISSION_ITEM\n");
            mavlink_mission_item_t msg;
            mavlink_msg_mission_item_decode(&message, &msg);
            listMissionItem.push(msg);
            break;
        }

        case MAVLINK_MSG_ID_MISSION_ITEM_REACHED:
		{
			//printf("MAVLINK_MSG_ID_MISSION_ITEM_REACHED\n");
			mavlink_mission_item_reached_t msg;
			mavlink_msg_mission_item_reached_decode(&message, &msg);
			listMissionItemReached.push(msg);
			break;
		}

        case MAVLINK_MSG_ID_MISSION_REQUEST_LIST:
        {
            //printf("MAVLINK_MSG_ID_MISSION_REQUEST_LIST\n");
            mavlink_mission_request_list_t msg;
            mavlink_msg_mission_request_list_decode(&message, &msg);
            listMissionRequestList.push(msg);
            break;
        }

        case MAVLINK_MSG_ID_MISSION_REQUEST:
        {
			//printf("MAVLINK_MSG_ID_MISSION_REQUEST\n");
			mavlink_mission_request_t msg;
			mavlink_msg_mission_request_decode(&message, &msg);
			listMissionRequest.push(msg);
			break;
		}

        
        case MAVLINK_MSG_ID_PARAM_REQUEST_LIST:
        {
            //printf("MAVLINK_MSG_ID_PARAM_REQUEST_LIST\n");
            mavlink_param_request_list_t msg;
            mavlink_msg_param_request_list_decode(&message, &msg);
            listParamRequestList.push(msg);
            break;
        }

        case MAVLINK_MSG_ID_PARAM_REQUEST_READ:
        {
            //printf("MAVLINK_MSG_ID_PARAM_REQUEST_READ\n");
            mavlink_param_request_read_t msg;
            mavlink_msg_param_request_read_decode(&message, &msg);
            listParamRequestRead.push(msg);
            break;
        }

        case MAVLINK_MSG_ID_PARAM_VALUE:
        {
            //printf("MAVLINK_MSG_ID_PARAM_SET\n");
            mavlink_param_value_t msg;
            mavlink_msg_param_value_decode(&message, &msg);
            listParamValue.push(msg);
            break;
        }

        case MAVLINK_MSG_ID_PARAM_SET:
        {
            //printf("MAVLINK_MSG_ID_PARAM_SET\n");
            mavlink_param_set_t msg;
            mavlink_msg_param_set_decode(&message, &msg);
            listParamSet.push(msg);
            break;
        }

        case MAVLINK_MSG_ID_COMMAND_LONG:
        {
            //printf("MAVLINK_MSG_ID_COMMAND_LONG\n");
            mavlink_command_long_t msg;
            mavlink_msg_command_long_decode(&message, &msg);
            listCommandLong.push(msg);
            break;
        }

        case MAVLINK_MSG_ID_COMMAND_INT:
        {
            //printf("MAVLINK_MSG_ID_COMMAND_INT\n");
            mavlink_command_int_t msg;
            mavlink_msg_command_int_decode(&message, &msg);
            listCommandInt.push(msg);
            break;
        }

        case MAVLINK_MSG_ID_SET_MODE:
        {
            //printf("MAVLINK_MSG_ID_SET_MODE\n");
            mavlink_set_mode_t msg;
            mavlink_msg_set_mode_decode(&message, &msg);
            listSetMode.push(msg);
            break;
        }

        case MAVLINK_MSG_ID_COMMAND_ACK:
        {
            //printf("MAVLINK_MSG_ID_COMMAND_ACK\n");
            mavlink_command_ack_t msg;
            mavlink_msg_command_ack_decode(&message, &msg);
            listCommandAck.push(msg);
            break;
        }

        case MAVLINK_MSG_ID_GPS_RAW_INT:
		{
			//printf("MAVLINK_MSG_ID_GPS_RAW_INT\n");
			mavlink_msg_gps_raw_int_decode(&message,&gpsRawInt);
			break;
		}

        case MAVLINK_MSG_ID_GLOBAL_POSITION_INT:
		{
			//printf("MAVLINK_MSG_ID_GLOBAL_POSITION_INT\n");
			mavlink_msg_global_position_int_decode(&message,&globalPositionInt);
			break;
		}

        case MAVLINK_MSG_ID_FENCE_POINT:
        {
        	//printf("MAVLINK_MSG_ID_FENCE_POINT\n");
        	mavlink_fence_point_t msg;
			mavlink_msg_fence_point_decode(&message,&msg);
			listFencePoint.push(msg);
			break;

        }

    }
    pthread_mutex_unlock(&lock);
 }


 bool MAVLinkMessages_t::GetMissionCount(mavlink_mission_count_t& msg){

     bool val;
     pthread_mutex_lock(&lock);
     if(!listMissionCount.empty()){
         msg = listMissionCount.front();
         listMissionCount.pop();
         val =  true;
     }
     else{
         val = false;
     }
     pthread_mutex_unlock(&lock);
     return val;   
 }

 bool MAVLinkMessages_t::GetMissionItem(mavlink_mission_item_t& msg){

     bool val;
     pthread_mutex_lock(&lock);
     if(!listMissionItem.empty()){
         msg = listMissionItem.front();
         listMissionItem.pop();
         val = true;
     }
     else{
         val = false;
     }
     pthread_mutex_unlock(&lock);
     return val;   
 }

 bool MAVLinkMessages_t::GetMissionRequestList(mavlink_mission_request_list_t& msg){

     bool val;
     pthread_mutex_lock(&lock);
     if(!listMissionRequestList.empty()){
         msg = listMissionRequestList.front();
         listMissionRequestList.pop();
         val = true;
     }
     else{
         val = false;
     }
     pthread_mutex_unlock(&lock);
     return val;  
 }

 bool MAVLinkMessages_t::GetMissionRequest(mavlink_mission_request_t& msg){

     bool val;
     pthread_mutex_lock(&lock);
     if(!listMissionRequest.empty()){
         msg = listMissionRequest.front();
         listMissionRequest.pop();
         val = true;
     }
     else{
         val = false;
     }
     pthread_mutex_unlock(&lock);
     return val;  
 }

 bool MAVLinkMessages_t::GetParamRequestList(mavlink_param_request_list_t& msg){

     bool val;
     pthread_mutex_lock(&lock);
     if(!listParamRequestList.empty()){
         msg = listParamRequestList.front();
         listParamRequestList.pop();
         val = true;
     }
     else{
         val = false;
     }
     pthread_mutex_unlock(&lock);
     return val;  
 }

bool MAVLinkMessages_t::GetParamSet(mavlink_param_set_t& msg){

     bool val;
     pthread_mutex_lock(&lock);
     if(!listParamSet.empty()){
         msg = listParamSet.front();
         listParamSet.pop();
         val = true;
     }
     else{
         val = false;
     }
     pthread_mutex_unlock(&lock);
     return val;  
 }

 bool MAVLinkMessages_t::GetParamValue(mavlink_param_value_t& msg){

     bool val;
     pthread_mutex_lock(&lock);
     if(!listParamValue.empty()){
         msg = listParamValue.front();
         listParamValue.pop();
         val = true;
     }
     else{
         val = false;
     }
     pthread_mutex_unlock(&lock);
     return val;  
 }

 bool MAVLinkMessages_t::GetParamRequestRead(mavlink_param_request_read_t& msg){

     bool val;
     pthread_mutex_lock(&lock);
     if(!listParamRequestRead.empty()){
         msg = listParamRequestRead.front();
         listParamRequestRead.pop();
         val = true;
     }
     else{
         val = false;
     }
     pthread_mutex_unlock(&lock);
     return val;  
 }

 bool MAVLinkMessages_t::GetSetMode(mavlink_set_mode_t& msg){

     bool val;
     pthread_mutex_lock(&lock);
     if(!listSetMode.empty()){
         msg = listSetMode.front();
         listSetMode.pop();
         val = true;
     }
     else{
         val = false;
     }
     pthread_mutex_unlock(&lock);
     return val;  
 }

 bool MAVLinkMessages_t::GetCommandLong(mavlink_command_long_t& msg){

     bool val;
     pthread_mutex_lock(&lock);
     if(!listCommandLong.empty()){
         msg = listCommandLong.front();
         listCommandLong.pop();
         val = true;
     }
     else{
         val = false;
     }
     pthread_mutex_unlock(&lock);
     return val;  
 }

 bool MAVLinkMessages_t::GetCommandInt(mavlink_command_int_t& msg){

     bool val;
     pthread_mutex_lock(&lock);
     if(!listCommandInt.empty()){
         msg = listCommandInt.front();
         listCommandInt.pop();
         val = true;
     }
     else{
         val = false;
     }
     pthread_mutex_unlock(&lock);
     return val;  
 }

 bool MAVLinkMessages_t::GetCommandAck(mavlink_command_ack_t& msg){
     bool val;
     pthread_mutex_lock(&lock);
     if(!listCommandAck.empty()){
         msg = listCommandAck.front();
         listCommandAck.pop();
         val = true;
     }
     else{
         val = false;
     }
     pthread_mutex_unlock(&lock);
     return val;  

 }

 bool MAVLinkMessages_t::GetFencePoint(mavlink_fence_point_t& msg){
	  bool val;
	  pthread_mutex_lock(&lock);
	  if(!listFencePoint.empty()){
		  msg = listFencePoint.front();
		  listFencePoint.pop();
		  val = true;
	  }
	  else{
		  val = false;
	  }
	  pthread_mutex_unlock(&lock);
	  return val;
 }

 void MAVLinkMessages_t::GetGPSRawInt(double& lat, double& lon, double& abs_alt){

	 pthread_mutex_lock(&lock);
	 lat    = (double) gpsRawInt.lat/1E7;
	 lon    = (double) gpsRawInt.lon/1E7;
	 abs_alt = (double) gpsRawInt.lon/1E3;
	 pthread_mutex_unlock(&lock);

 }

 void MAVLinkMessages_t::GetGlobalPositionInt(double &time,double& lat, double& lon, double& abs_alt, double& rel_alt,
		  	  	  	  	  	  	  	  	  double& vx, double& vy, double& vz){
	 pthread_mutex_lock(&lock);
	 time   = (double) globalPositionInt.time_boot_ms/1E6;
	 lat    = (double) globalPositionInt.lat/1E7;
	 lon    = (double) globalPositionInt.lon/1E7;
	 abs_alt = (double) globalPositionInt.alt/1E3;
	 rel_alt = (double) globalPositionInt.relative_alt/1E3;
	 vx = (double) globalPositionInt.vx/1E2;
	 vy = (double) globalPositionInt.vy/1E2;
	 vz = (double) globalPositionInt.vz/1E2;
	 pthread_mutex_unlock(&lock);
 }

 void MAVLinkMessages_t::GetAttitude(double& roll,double& pitch,double& yaw){

	 pthread_mutex_lock(&lock);
	 roll  = (double) attitude.roll;
	 pitch = (double) attitude.pitch;
	 yaw   = (double) attitude.yaw;
	 pthread_mutex_unlock(&lock);
 }

 bool MAVLinkMessages_t::GetMissionItemReached(mavlink_mission_item_reached_t& msg){
	 bool val;
	 pthread_mutex_lock(&lock);
	 if(!listMissionItemReached.empty()){
		 msg = listMissionItemReached.front();
		 listMissionItemReached.pop();
		 val = true;
	 }
	 else{
		 val = false;
	 }
	 pthread_mutex_unlock(&lock);
	 return val;
 }
