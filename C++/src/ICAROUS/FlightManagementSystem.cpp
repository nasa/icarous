/**
 * Flight Management System
 * 
 * Core flight management functions
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

 #include "FlightManagementSystem.h"

FlightManagementSystem::FlightManagementSystem(Interface *px4int, Interface *gsint,AircraftData* fData){
    px4Intf      = px4int;
    gsIntf       = gsint;
    FlightData   = fData;
    RcvdMessages = fData->RcvdMessages; 
    fmsState     = _idle_;
}

void FlightManagementSystem::RunFMS(){
     while(true){
        switch(fmsState){
            case _idle_:
                IDLE();
                break;

            case _takeoff_:
                TAKEOFF();
                break;

            case _climb_:
                CLIMB();
                break;

            case _cruise_:
                CRUISE();
                break;

            case _descend_:
                DESCEND();
                break;

            case _land_:
                LAND();
                break;
        }
     }
 }

 void FlightManagementSystem::
 SendCommand(uint8_t target_system,uint8_t target_component,
             uint16_t command,uint8_t confirmation,
             float param1, float param2, float param3,float param4, 
             float param5, float param6, float param7){

    mavlink_message_t msg;
    mavlink_msg_command_long_pack(255,0,&msg,target_system,target_component,
                                  command,confirmation,param1,param2,param3,
                                  param4,param5,param6,param7);

    px4Intf->SendMAVLinkMsg(msg);
}

void FlightManagementSystem::SetYaw(double heading){
    SendCommand(0,0,MAV_CMD_CONDITION_YAW,0,
		       (float)heading,0,1,0,
		       0,0,0);
}

void FlightManagementSystem::SetGPSPos(double lat,double lon, double alt){
    mavlink_message_t msg;
    mavlink_msg_set_position_target_global_int_pack(255, 0, &msg,0,1,0, 
                                                    MAV_FRAME_GLOBAL_RELATIVE_ALT_INT, 
                                                    0b0000111111111000, 
                                                    (int32_t) (lat*1E7), (int32_t) (lon*1E7), (float) alt, 
                                                    0, 0, 0, 0, 0, 0, 0, 0);
    px4Intf->SendMAVLinkMsg(msg);
}

void FlightManagementSystem::SetVelocity(double Vn,double Ve,double Vu){
    mavlink_message_t msg;
    mavlink_msg_set_position_target_local_ned_pack(255, 0, &msg,0,1,0, 
                                                    MAV_FRAME_LOCAL_NED, 
                                                    0b0000111111000111, 
                                                    0 , 0, 0, 
                                                    (float) Vn, (float) Ve, (float) Vu, 
                                                    0, 0, 0, 
                                                    0, 0);

    px4Intf->SendMAVLinkMsg(msg);
}

void FlightManagementSystem::SetMode(control_mode_t mode){
    mavlink_message_t msg;
    mavlink_msg_set_mode_pack(255,0,&msg,
                              0,1,mode);
	
    px4Intf->SendMAVLinkMsg(msg);
}

void FlightManagementSystem::SetSpeed(float speed){
	
	SendCommand(0,0,MAV_CMD_DO_CHANGE_SPEED,0,
		        1,speed,0,0,0,0,0);
}