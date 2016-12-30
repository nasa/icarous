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


FlightManagementSystem_t::FlightManagementSystem_t(Interface_t *px4int, Interface_t *gsint,AircraftData_t* fData,Mission_t *task):log("FMS"){
    px4Intf      = px4int;
    gsIntf       = gsint;
    FlightData   = fData;
    RcvdMessages = fData->RcvdMessages; 
    fmsState     = _idle_;
    conflictSize = 0;
    mission      = task;
    deviationApproved = false;
    landStarted  = false;
}

void FlightManagementSystem_t::RunFMS(){
     while(true){

    	GetLatestAircraftData();
    	CheckMissionWaypointReached();

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

 void FlightManagementSystem_t::
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

void FlightManagementSystem_t::SetYaw(double heading){
    SendCommand(0,0,MAV_CMD_CONDITION_YAW,0,
		       (float)heading,0,1,0,
		       0,0,0);
}

void FlightManagementSystem_t::SetGPSPos(double lat,double lon, double alt){
    mavlink_message_t msg;
    mavlink_msg_set_position_target_global_int_pack(255, 0, &msg,0,1,0, 
                                                    MAV_FRAME_GLOBAL_RELATIVE_ALT_INT, 
                                                    0b0000111111111000, 
                                                    (int32_t) (lat*1E7), (int32_t) (lon*1E7), (float) alt, 
                                                    0, 0, 0, 0, 0, 0, 0, 0);
    px4Intf->SendMAVLinkMsg(msg);
}

void FlightManagementSystem_t::SetVelocity(double Vn,double Ve,double Vu){
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

void FlightManagementSystem_t::SetMode(control_mode_t mode){
    mavlink_message_t msg;
    mavlink_msg_set_mode_pack(255,0,&msg,
                              0,1,mode);
	
    px4Intf->SendMAVLinkMsg(msg);
}

void FlightManagementSystem_t::SetSpeed(float speed){
	
	SendCommand(0,0,MAV_CMD_DO_CHANGE_SPEED,0,
		        1,speed,0,0,0,0,0);

	FlightData->speed = speed;
}

void FlightManagementSystem_t::SendStatusText(char buffer[]){
    mavlink_message_t msg;
    mavlink_msg_statustext_pack(1,0,&msg,MAV_SEVERITY_INFO,buffer);
    gsIntf->SendMAVLinkMsg(msg);
}

void FlightManagementSystem_t::ArmThrottles(bool arm){

    uint8_t c;

    if(arm){
        c = 1;
    }else{
        c = 0;
    }

    SendCommand(0,0,MAV_CMD_COMPONENT_ARM_DISARM,0,
			         (float)c,0,0,0,0,0,0);
}

void FlightManagementSystem_t::StartTakeoff(float alt){
    SendCommand(0,0,MAV_CMD_NAV_TAKEOFF,0,
			    1,0,0,0,0,0,alt);
}

void FlightManagementSystem_t::StartLand(){
    SendCommand(0,0,MAV_CMD_NAV_LAND,0,
			    0,0,0,0,0,0,0);
}


bool FlightManagementSystem_t::CheckAck(MAV_CMD command){
    bool have_msg = true;
    bool status = false;
    mavlink_command_ack_t msg;

    while(have_msg){
       have_msg = RcvdMessages->GetCommandAck(msg);
       if(msg.command == command && msg.result == 0){
           status = true;
           return status;
       }
    }
    return status;
}

void FlightManagementSystem_t::GetLatestAircraftData(){

	// Get aircraft position data
	double lat,lon,abs_alt,rel_alt,vx,vy,vz,time;
	RcvdMessages->GetGlobalPositionInt(time,lat,lon,abs_alt,rel_alt,vx,vy,vz);
	Position currentPos = Position::makeLatLonAlt(lat,"degree",lon,"degree",rel_alt,"m");
	Velocity currentVel = Velocity::makeVxyz(vy,vx,"m/s",vz,"m/s");

	FlightData->acState.add(currentPos,currentVel,time);

	// Get aircraft attitude data
	double roll, pitch, yaw, heading;
	RcvdMessages->GetAttitude(roll,pitch,yaw);

	heading = currentVel.track("degree");
	if(heading < 0){
		heading = 360 + heading;
	}

	FlightData->roll = roll;
	FlightData->pitch = pitch;
	FlightData->yaw = yaw;
	FlightData->heading = heading;
}

uint8_t FlightManagementSystem_t::IDLE(){

    int start  = FlightData->GetStartMissionFlag();
    int fpsize = FlightData->GetFlightPlanSize(); 
    if( start == 0){
        if( fpsize > 0){
        	PREFLIGHT();
            fmsState = _takeoff_;
            FlightData->nextMissionWP = 0;
            return 1; 
        }
        else{
            SendStatusText("No flightplan uploaded");
            return 0;
        }
    }
    else if(start > 0 && start < fpsize ){
    	PREFLIGHT();
        fmsState           = _cruise_;
        FlightData->nextMissionWP = start;
        SendStatusText("Flying to waypoint");
        return 1;
    }

    return 0;
}

bool FlightManagementSystem_t::CheckMissionWaypointReached(){

	mavlink_mission_item_reached_t msg;
	bool val;
	val = RcvdMessages->GetMissionItemReached(msg);

	if(val){
		FlightData->nextMissionWP++;
	}

	return val;
}

uint8_t FlightManagementSystem_t::PREFLIGHT(){

	FlightData->ConstructPlan();
	return 0;
}

double FlightManagementSystem_t::GetApproxElapsedPlanTime(Plan fp,int nextWP){

	Position pos          = FlightData->acState.positionLast();
	double legDistance    = fp.pathDistance(nextWP - 1);
	double legTime        = fp.getTime(nextWP) - fp.getTime(nextWP-1);
	double lastWPDistance = fp.point(nextWP-1).position().distanceH(pos);
	double currentTime    = fp.getTime(nextWP-1) + legTime/legDistance * lastWPDistance;

	return currentTime;
}

void FlightManagementSystem_t::CheckReset(){

	if(FlightData->reset){
		FlightData->reset = false;
		Reset();
	}
}

void FlightManagementSystem_t::SetDeviationApproved(bool status){
	deviationApproved = status;
}
