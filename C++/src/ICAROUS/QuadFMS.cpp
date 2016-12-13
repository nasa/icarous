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
     targetAlt         = 0.0f;
     resolutionState   = IDLE_r;
     maneuverState     = IDLE_m;
     trajectoryState   = IDLE_t;
     planType          = MISSION;
     resumeMission     = true;
     DAA.parameters.loadFromFile("params/DaidalusQuadConfig.txt");
     time(&daaTimeStart);
     daaLookAhead = DAA.parameters.getLookaheadTime("s");
     trafficResolutionTime = 0;

 }

 QuadFMS_t::~QuadFMS_t(){}

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
		FlightData->nextMissionWP++;
		fmsState = _cruise_;
		SendStatusText("Starting cruise");
		return 1;
	}

	return 0;
}

uint8_t QuadFMS_t::CRUISE(){

	Monitor();

	Resolve();
	return 0;
}

uint8_t QuadFMS_t::DESCEND(){
	return 0;
}

uint8_t QuadFMS_t::LAND(){
	return 0;
}

uint8_t QuadFMS_t::Monitor(){

	//Monitor geofences
	CheckGeofence();

	//Check flight plan deviaiton
	CheckFlightPlanDeviation();

	//Check traffic
	if(FlightData->trafficList.size() > 0){
		CheckTraffic();

		time_t currentTime;
		time(&currentTime);
		double diff = difftime(currentTime,trafficResolutionTime);

		if( (Conflict.traffic == true) && (diff > 20) ){
			resolutionState = COMPUTE_r;
			printf("diff %f\n",diff);
			printf("resetting resolution to compute\n");
		}
	}

	if(Conflict.size() != conflictSize){
		conflictSize = Conflict.size();
		if(conflictSize > 0){
			resolutionState = COMPUTE_r;
		}
	}

	return conflictSize;
}

uint8_t QuadFMS_t::Resolve(){

	uint8_t status;



	switch(resolutionState){

	case COMPUTE_r:

		if(Conflict.traffic){
			printf("Computing traffic resolution\n");
			ResolveTrafficConflict();
			time(&trafficResolutionTime);
		}
		else if(Conflict.keepin){
			printf("Computing keep in resolution\n");
			ResolveKeepInConflict();
		}
		else if(Conflict.keepout){
			printf("Computing keep out resolution\n");
			ResolveKeepOutConflict();
		}
		else if(Conflict.flightPlanDeviation){
			printf("Computing standoff deviation\n");
			ResolveFlightPlanDeviation();
		}

		if(planType == TRAJECTORY){
			resolutionState = TRAJECTORY_r;
			trajectoryState = START_t;
		}
		else if(planType == MANEUVER){
			resolutionState = MANEUVER_r;
			maneuverState = START_m;
		}

		break;
	case MANEUVER_r:
		// Do something
		status = FlyManuever();
		if(status == 1){
			resolutionState = IDLE_r;
			SetMode(AUTO);
		}
		break;

	case TRAJECTORY_r:
		// Fly trajectory

		status = FlyTrajectory();

		if(status == 1){
			if(resumeMission){
				resolutionState = IDLE_r;
				SetMode(AUTO);
			}
			else{
				resolutionState = RESUME_r;
			}
		}

		break;

	case RESUME_r:
		// Resume mission
		ComputeInterceptCourse();
		resolutionState = TRAJECTORY_r;
		trajectoryState = START_t;
		Conflict.clear();
		conflictSize = Conflict.size();

		break;
	case IDLE_r:
		break;
	}

	return 0;
}

uint8_t QuadFMS_t::FlyTrajectory(){

	uint8_t status = 0;
	float resolutionSpeed;
	NavPoint wp;
	Position current, next;
	double distH,distV;

	switch(trajectoryState){

	case START_t:
		printf("executing trajectory resolution\n");
		resolutionSpeed = FlightData->paramData->getValue("RES_SPEED");
		SetMode(GUIDED); sleep(1);
		SetSpeed(resolutionSpeed);  usleep(5000);
		trajectoryState = FIX_t;
		break;

	case FIX_t:
		wp = FlightData->ResolutionPlan.point(FlightData->nextResolutionWP);
		SetGPSPos(wp.lla().latitude(),wp.lla().longitude(),wp.lla().alt());
		trajectoryState = ENROUTE_t;
		//printf("Sent command\n");
		break;

	case ENROUTE_t:
		current = FlightData->acState.positionLast();
		next    = FlightData->ResolutionPlan.point(FlightData->nextResolutionWP).position();
		distH     = current.distanceH(next);
		distV     = current.distanceV(next);
		//printf("distH,V = %f,%f\n",distH,distV);
		if(distH < 1 && distV < 0.5){
			FlightData->nextResolutionWP++;
			if(FlightData->nextResolutionWP >= FlightData->ResolutionPlan.size()){
				trajectoryState = STOP_t;
				FlightData->nextResolutionWP = 0;
			}
			else{
				trajectoryState = FIX_t;
			}
		}

		break;

	case STOP_t:
		FlightData->nextResolutionWP = 0;
		FlightData->ResolutionPlan.clear();
		trajectoryState = IDLE_t;
		status = 1;
		break;

	case IDLE_t:
		break;
	}

	return status;
}

uint8_t QuadFMS_t::FlyManuever(){
	uint8_t status = 0;
	float resolutionSpeed = FlightData->paramData->getValue("RES_SPEED");

	switch(maneuverState){

	case START_m:
		printf("executing maneuver resolution\n");
		SetMode(GUIDED);
		SetSpeed(resolutionSpeed);
		maneuverState = GUIDE_m;
		break;

	case GUIDE_m:
		if(Conflict.flightPlanDeviation){
			ResolveFlightPlanDeviation();
			SetYaw(FlightData->manueverHeading);
			SetVelocity(FlightData->manueverVn,FlightData->manueverVe,FlightData->manueverVu);
		}

		if(!Conflict.flightPlanDeviation){
			printf("finished maneuver resolution\n");
			maneuverState = IDLE_m;
			Conflict.clear();
			conflictSize = Conflict.size();
		}
		break;

	case IDLE_m:
		status = 1;
		break;

	}

	return status;
}

void QuadFMS_t::ComputeInterceptCourse(){
	Position current = FlightData->acState.positionLast();
	Position next    = FlightData->MissionPlan.point(FlightData->nextMissionWP).position();

	double distH = current.distanceH(next);
	float speed  = FlightData->paramData->getValue("RES_SPEED");
	double ETA   = distH/speed;

	NavPoint wp1(current,0);
	NavPoint wp2(next,ETA);
	FlightData->ResolutionPlan.add(wp1);
	FlightData->ResolutionPlan.add(wp2);
	FlightData->nextResolutionWP = 0;
	planType      = TRAJECTORY;
	resumeMission = true;
}

