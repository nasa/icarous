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
#include <sys/time.h>

FlightManagementSystem_t::FlightManagementSystem_t(AircraftData_t* fData){
    FlightData   = fData;
    fmsState     = _idle_;
    conflictSize = 0;
    deviationApproved = false;
    landStarted  = false;
    icarousActive = true;
    currentMode   = _PASSIVE_;
    sendTkCommand     = 0;
}

void FlightManagementSystem_t::RunFMS(){



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

	CheckReset();

 }

 void FlightManagementSystem_t::
 SendCommand(uint8_t target_system,uint8_t target_component,
             uint16_t command,uint8_t confirmation,
             float param1, float param2, float param3,float param4, 
             float param5, float param6, float param7){


}

void FlightManagementSystem_t::SetYaw(bool relative,double heading){
	int rel = relative?1:0;

	ArgsCmd_t cmd;
	cmd.name = _SETYAW_;
	cmd.param1 = heading;
	cmd.param2 = 0;
	cmd.param3 = 1;
	cmd.param4 = rel;
	FlightData->commandList.push_back(cmd);
}

void FlightManagementSystem_t::SetGPSPos(double lat,double lon, double alt){
	ArgsCmd_t cmd;
	cmd.name = _SETPOS_;
	cmd.param1 = lat;
	cmd.param2 = lon;
	cmd.param3 = alt;
	FlightData->commandList.push_back(cmd);
}

void FlightManagementSystem_t::SetVelocity(double Vn,double Ve,double Vu){
	ArgsCmd_t cmd;
	cmd.name = _SETVEL_;
	cmd.param1 = Vn;
	cmd.param2 = Ve;
	cmd.param3 = Vu;
	FlightData->commandList.push_back(cmd);
}

void FlightManagementSystem_t::SetMode(icarous_control_mode_t mode){
	ArgsCmd_t cmd;
	cmd.name = _SETMODE_;
	cmd.param1 = mode;
	FlightData->commandList.push_back(cmd);
}

void FlightManagementSystem_t::SetMissionItem(uint8_t nextWP){
	ArgsCmd_t cmd;
	cmd.name = _GOTOWP_;
	cmd.param1 = nextWP;
	FlightData->commandList.push_back(cmd);
}

void FlightManagementSystem_t::SetSpeed(float speed){
	ArgsCmd_t cmd;
	cmd.name = _SETSPEED_;
	cmd.param1 = speed;
	FlightData->commandList.push_back(cmd);
	FlightData->speed = speed;
}

void FlightManagementSystem_t::SendStatusText(char buffer[]){
	cout<<buffer<<endl;
}

void FlightManagementSystem_t::ArmThrottles(bool arm){

    uint8_t c;

    if(arm){
        c = 1;
    }else{
        c = 0;
    }

    ArgsCmd_t cmd;
    cmd.name = _ARM_;
    cmd.param1 = c;
    FlightData->commandList.push_back(cmd);
}

void FlightManagementSystem_t::StartTakeoff(float alt){
	ArgsCmd_t cmd;
	cmd.name = _TAKEOFF_;
	cmd.param1 = alt;
	FlightData->commandList.push_back(cmd);
}

void FlightManagementSystem_t::StartLand(){
	ArgsCmd_t cmd;
	cmd.name = _LAND_;
	Position currPosition = FlightData->acState.positionLast();
	cmd.param5 = currPosition.latitude();
	cmd.param6 = currPosition.longitude();
	cmd.param7 = currPosition.alt();
	FlightData->commandList.push_back(cmd);
}


bool FlightManagementSystem_t::CheckAck(command_name_t command){

	bool status = false;
	 while(FlightData->commandAckList.size() > 0){
		   CmdAck_t ack = FlightData->commandAckList.front();
		   FlightData->commandAckList.pop_front();

	       if(ack.name == command && ack.result == 0){
	           status = true;
	           return status;
	       }
	    }
	    return status;

}

void FlightManagementSystem_t::GetCurrentMode(){

}

uint8_t FlightManagementSystem_t::IDLE(){

    int start  = FlightData->GetStartMissionFlag();
    int fpsize = FlightData->GetFlightPlanSize(); 
    if( start == 0){
        if( fpsize > 0){
        	PREFLIGHT();
            fmsState = _takeoff_;
            FlightData->nextMissionWP = 0;
            start = -1;
            sendTkCommand = -1;
            return 1; 
        }
        else{
            SendStatusText("No flightplan uploaded");
            start = -1;
            return 0;
        }
    }
    else if(start > 0 && start < fpsize ){
    	PREFLIGHT();
        fmsState           = _cruise_;
        FlightData->nextMissionWP = start;
        SendStatusText("Flying to waypoint");
        start = -1;
        return 1;
    }else{
    	start = -1;
    }

    return 0;
}



bool FlightManagementSystem_t::CheckMissionWaypointReached(){

	return FALSE;
}

uint8_t FlightManagementSystem_t::PREFLIGHT(){

	FlightData->ConstructPlan();
	return 0;
}

double FlightManagementSystem_t::GetApproxElapsedPlanTime(Plan fp,int nextWP){

	Position pos          = FlightData->acState.positionLast();
	double legDistance    = fp.pathDistance(nextWP - 1);
	double legTime        = fp.time(nextWP) - fp.time(nextWP-1);
	double lastWPDistance = fp.point(nextWP-1).position().distanceH(pos);
	double currentTime    = fp.time(nextWP-1) + legTime/legDistance * lastWPDistance;
	//printf("%d, %f, %f\n",nextWP,lastWPDistance,currentTime);
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
