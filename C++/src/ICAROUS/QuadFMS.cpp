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

 #include "QuadFMS.h"

 QuadFMS_t::QuadFMS_t(Interface_t *px4int, Interface_t *gsint,AircraftData_t* fData):
 FlightManagementSystem_t(px4int,gsint,fData){
     targetAlt       = 0.0f;
 }

uint8_t QuadFMS_t::TAKEOFF(){

	targetAlt = (float) FlightData->paramData->getValue("TAKEOFF_ALT");

	SetMode(GUIDED);

	// Arm throttles
	ArmThrottles(true);
        
	// Send Takeoff with target altitude
	StartTakeoff(targetAlt);

	// Sleep
	sleep(1);

	if(CheckAck(MAV_CMD_NAV_TAKEOFF)){
		fmsState = _climb_;
		SendStatusText("Starting climb");
		return 1;
	}
	else{
		fmsState = _idle_;
		return 0;
	}
}

uint8_t QuadFMS_t::CLIMB(){

	double currentAlt = FlightData->acState.positionLast().alt();
	double alt_error  = fabs(currentAlt - targetAlt);

	if( alt_error < 0.5 ){
		SetMode(AUTO);
		SetSpeed(1.0f);
		FlightData->nextWP++;
		fmsState = _cruise_;
		SendStatusText("Starting cruise");
		return 1;
	}

	return 0;
}

uint8_t QuadFMS_t::CRUISE(){

	return 0;
}

uint8_t QuadFMS_t::DESCEND(){
	return 0;
}

uint8_t QuadFMS_t::LAND(){
	return 0;
}

uint8_t QuadFMS_t::Monitor(){

	Position CurrentPos = FlightData->acState.positionLast();
	Velocity CurrentVel = FlightData->acState.velocityLast();

	Conflict.keepin = false;
	Conflict.keepout = false;

	// Check for geofence violation
	for(FlightData->fenceListIt = FlightData->fenceList.begin();
		FlightData->fenceListIt != FlightData->fenceList.end();
		++FlightData->fenceListIt){
		Geofence_t fence = *FlightData->fenceListIt;
		fence.CheckViolation(FlightData->acState);

		if(fence.GetProjectedStatus() || fence.GetConflictStatus() || fence.GetViolationStatus()){
			Conflict.AddConflict(fence);
			if(fence.GetType() == KEEP_IN){
				Conflict.keepin = true;
			}
			else{
				Conflict.keepout = true;
			}
		}
	}

	return Conflict.size();
}

uint8_t QuadFMS_t::Resolve(){

	return 0;
}
