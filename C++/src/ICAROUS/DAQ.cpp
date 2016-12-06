/**
 * Data acquisition
 * 
 * This class contains functions to constantly collect data from the pixhawk.
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

#include "DAQ.h"

 DataAcquisition::DataAcquisition(Interface* px4int, Interface* gsint,AircraftData *fdata){
     px4Intf      = px4int;
     gsIntf       = gsint;
     FlightData   = fdata;
     RcvdMessages = fdata->RcvdMessages;
     WPcount      = 0;                     // total waypoint count
     WPloaded     = 0;                     // waypoints loaded
 }

// Get data from pixhawk
 void DataAcquisition::GetPixhawkData(){
     while(true){
         // Get data from the pixhawk
         px4Intf->GetMAVLinkMsg();
         
         // Send the raw data in the queue to the send interface
         while(!px4Intf->msgQueue.empty()){
            gsIntf->SendMAVLinkMsg(px4Intf->msgQueue.front());
            px4Intf->msgQueue.pop();
         }
     }
 }


// Get data from ground station
 void DataAcquisition::GetGSData(){

     
     while(true){
         gsIntf->GetMAVLinkMsg();
         
         
         // Handle mission count
         MissionCountHandler();

         // Mission item handler
         MissionItemHandler();

         // Handle mission list request
         MissionRequestListHandler();

         //  Handle mission request
         MissionRequestHandler();

         // Handle parameter request list
         ParamRequestListHandler();

         // Handle parameter request read
         ParamRequestReadHandler();

         // Handle parameter set
         ParamSetHandler();

         // Handle set mode
         SetModeHandler();

         // Handle long commands
         CommandLongHandler();

     }
     

 }

 
 void DataAcquisition::MissionCountHandler(){
     mavlink_mission_count_t msg;
     bool have_msg = RcvdMessages->GetMissionCount(msg);
     if(have_msg && msg.target_system == 1){
         mavlink_message_t msg2send;
         mavlink_msg_mission_count_encode(255,0,&msg2send,&msg);
         px4Intf->SendMAVLinkMsg(msg2send);
         WPcount = msg.count;
         
     }

 }

 void DataAcquisition::MissionItemHandler(){
     mavlink_mission_item_t msg;
     bool have_msg = RcvdMessages->GetMissionItem(msg);
     if(have_msg && msg.target_system == 1){
         mavlink_message_t msg2send;
         mavlink_msg_mission_item_encode(255,0,&msg2send,&msg);
         px4Intf->SendMAVLinkMsg(msg2send);
         if(msg.seq == WPloaded){
             //printf("Added wp %d to list\n",WPloaded);
             FlightData->AddMissionItem(msg);
             WPloaded++;
         }
     }
 }

 void DataAcquisition::MissionRequestHandler(){
     mavlink_mission_request_t msg;
     bool have_msg = RcvdMessages->GetMissionRequest(msg);
     if(have_msg && msg.target_system == 1){
         mavlink_message_t msg2send;
         mavlink_msg_mission_request_encode(255,0,&msg2send,&msg);
         px4Intf->SendMAVLinkMsg(msg2send);
     }
 }

 void DataAcquisition::MissionRequestListHandler(){
     mavlink_mission_request_list_t msg;
     bool have_msg = RcvdMessages->GetMissionRequestList(msg);
     if(have_msg && msg.target_system == 1){
         mavlink_message_t msg2send;
         mavlink_msg_mission_request_list_encode(255,0,&msg2send,&msg);
         px4Intf->SendMAVLinkMsg(msg2send);
     }
 }

 void DataAcquisition::ParamRequestListHandler(){
     mavlink_param_request_list_t msg;
     bool have_msg = RcvdMessages->GetParamRequestList(msg);
     if(have_msg){
         mavlink_message_t msg2send;
         mavlink_msg_param_request_list_encode(255,0,&msg2send,&msg);
         px4Intf->SendMAVLinkMsg(msg2send);
     }
 }

 void DataAcquisition::ParamRequestReadHandler(){
     mavlink_param_request_read_t msg;
     bool have_msg = RcvdMessages->GetParamRequestRead(msg);
     if(have_msg && msg.target_system == 1){
         mavlink_message_t msg2send;
         mavlink_msg_param_request_read_encode(255,0,&msg2send,&msg);
         px4Intf->SendMAVLinkMsg(msg2send);
     }
 }

 void DataAcquisition::ParamSetHandler(){
     mavlink_param_set_t msg;
     bool have_msg = RcvdMessages->GetParamSet(msg);
     if(have_msg && msg.target_system == 1){
         mavlink_message_t msg2send;
         mavlink_msg_param_set_encode(255,0,&msg2send,&msg);
         px4Intf->SendMAVLinkMsg(msg2send);
     }
 }

  void DataAcquisition::ParamValueHandler(){
     mavlink_param_value_t msg;
     bool have_msg = RcvdMessages->GetParamValue(msg);
     if(have_msg){
         mavlink_message_t msg2send;
         mavlink_msg_param_value_encode(255,0,&msg2send,&msg);
         
     }
 }

  void DataAcquisition::SetModeHandler(){
     mavlink_set_mode_t msg;
     bool have_msg = RcvdMessages->GetSetMode(msg);
     if(have_msg && msg.target_system == 1){
         mavlink_message_t msg2send;
         mavlink_msg_set_mode_encode(255,0,&msg2send,&msg);
         px4Intf->SendMAVLinkMsg(msg2send);
     }
 }

 void DataAcquisition::CommandLongHandler(){
     mavlink_command_long_t msg;
     bool have_msg = RcvdMessages->GetCommandLong(msg);
     if(have_msg && msg.command == MAV_CMD_MISSION_START){
         FlightData->SetStartMissionFlag((uint8_t)msg.param1);
     }
     else if(have_msg && msg.command == MAV_CMD_DO_FENCE_ENABLE){
    	 FlightData->GetGeofence(gsIntf,msg);
     }
     else if(have_msg){
    	 mavlink_message_t msg2send;
		 mavlink_msg_command_long_encode(255,0,&msg2send,&msg);
    	 px4Intf->SendMAVLinkMsg(msg2send);
     }
 }

 void DataAcquisition::CommandIntHandler(){
     mavlink_command_int_t msg;
     bool have_msg = RcvdMessages->GetCommandInt(msg);
     if(have_msg){
         
     }
 }
