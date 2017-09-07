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

 QuadFMS_t::QuadFMS_t(AircraftData_t* fData):
 FlightManagementSystem_t(fData){
     targetAlt         = 0.0f;
     resolutionState   = IDLE_r;
     maneuverState     = IDLE_m;
     trajectoryState   = IDLE_t;
     planType          = MISSION;
     resumeMission     = true;
     captureH          = (float) FlightData->paramData->getValue("CAPTURE_H");
     captureV          = (float) FlightData->paramData->getValue("CAPTURE_V");
     newSolution       = false;
 }

QuadFMS_t::~QuadFMS_t(){}

uint8_t QuadFMS_t::TAKEOFF(){

    if(startTakeoffTime <= 0.0) {
        targetAlt = (float) FlightData->paramData->getValue("TAKEOFF_ALT");

        SetMode(_ACTIVE_);

        // Arm throttles
        ArmThrottles(true);

        // Send Takeoff with target altitude
        StartTakeoff(targetAlt);

        startTakeoffTime = FlightData->acTime;
    }else {
        if(FlightData->acTime - startTakeoffTime > 2) {
            if (CheckAck(_TAKEOFF_)) {
                fmsState = _climb_;
                SendStatusText("Starting climb", 15);
                return 1;
            } else {
                printf("Takeoff failed\n");
                fmsState = _idle_;
                return 0;
            }
        }
    }
}

uint8_t QuadFMS_t::CLIMB(){

	double currentAlt = FlightData->acState.positionLast().alt();
	double alt_error  = std::abs(currentAlt - targetAlt);

	if( alt_error < 0.5 ){
		SetMode(_PASSIVE_);
		//SetSpeed(1.0f);
		FlightData->nextMissionWP++;
		fmsState = _cruise_;
		SendStatusText("Starting cruise",16);
		return 1;
	}

	return 0;
}

uint8_t QuadFMS_t::CRUISE(){

	int confSize;

	confSize = Monitor();
	if(confSize != conflictSize){
		//printf("Conflict size %d,%d\n",confSize,conflictSize);
		conflictSize = confSize;
		if(conflictSize > 0){
			resolutionState = COMPUTE_r;
		}
	}

	if(resolutionState != IDLE_r && icarousActive){
		Resolve();
	}
	else if(!icarousActive){
		resolutionState = IDLE_r;
		trajectoryState = IDLE_t;
		maneuverState   = IDLE_m;
	}


	if(FlightData->nextMissionWP >= FlightData->MissionPlan.size()){
		fmsState = _land_;
	}

	return 0;
}

uint8_t QuadFMS_t::DESCEND(){
	return 0;
}

uint8_t QuadFMS_t::LAND(){

	if(!landStarted){
		SendStatusText("Landing",8);
		SetMode(_ACTIVE_);
		StartLand();
		landStarted = true;
	}
	return 0;
}

uint8_t QuadFMS_t::Monitor(){

	//Monitor geofences
	Detector.CheckGeofence();

	//Check flight plan deviaiton
	Detector.CheckFlightPlanDeviation(deviationApproved);

	// Check traffic conflicts
	Detector.CheckTraffic();

	return Detector.size();
}

uint8_t QuadFMS_t::Resolve(){

	uint8_t status;

	time_t startTime;
	time_t stopTime;

	switch(resolutionState){

	case COMPUTE_r:

		if(Detector.trafficConflict){
			printf("Computing traffic resolution\n");
			SendStatusText("traffic conflict",17);
			int search_type = FlightData->paramData->getInt("CHEAP_DAA");
			if(search_type == 0){
				Resolver.ResolveTrafficConflictRRT();
			}
			else{
				Resolver.ResolveTrafficConflictDAA();
			}
		}
		else if(Detector.keepInConflict){
			SendStatusText("keep in conflict",17);
			printf("Computing keep in resolution\n");
			Resolver.ResolveKeepInConflict();
		}
		else if(Detector.keepOutConflict){
			SetMode(_ACTIVE_);
			SendStatusText("keep out conflict",18);
			int search_type = FlightData->paramData->getInt("CHEAP_SEARCH");
			printf("Computing keep out resolution\n");
			time(&startTime);
			if(search_type == 0){
				Resolver.ResolveKeepOutConflict_Astar();
			}
			else{
				Resolver.ResolveKeepOutConflict_RRT();
			}
			time(&stopTime);
			printf("Time to compute solution %f\n",difftime(stopTime,startTime));
		}
		else if(Detector.flightPlanDeviationConflict){
			SendStatusText("flight plan deviation",22);
			printf("Computing standoff deviation\n");
			Resolver.ResolveFlightPlanDeviation();
		}

		if(planType == TRAJECTORY){
			resolutionState = TRAJECTORY_r;
			newSolution = true;
		}
		else if(planType == MANEUVER){
			resolutionState = MANEUVER_r;
			newSolution = true;
		}

		break;
	case MANEUVER_r:
		// Do something
		status = FlyManeuver(newSolution);
		newSolution = false;
		if(status == 1){
			resolutionState = IDLE_r;
			SetMissionItem(FlightData->nextMissionWP);
			planType = QuadFMS_t::MISSION;
			cout<<"Next mission item: "<<FlightData->nextMissionWP<<endl;
			SetMode(_PASSIVE_);
		}
		break;

	case TRAJECTORY_r:
		// Fly trajectory

		status = FlyTrajectory(newSolution);
		newSolution = false;

		if(status == 1){
			if(resumeMission){
				resolutionState = IDLE_r;
				SetMissionItem(FlightData->nextMissionWP);
				SetSpeed(FlightData->getFlightPlanSpeed(&FlightData->MissionPlan,FlightData->nextMissionWP));
				planType = QuadFMS_t::MISSION;
				SetMode(_PASSIVE_);
			}
			else{
				resolutionState = RESUME_r;
			}
		}

		break;

	case RESUME_r:
		// Resume mission
		printf("Resuming mission\n");
		ComputeInterceptCourse();
		resolutionState = TRAJECTORY_r;
		trajectoryState = START_t;
		Detector.clear();
		conflictSize = Detector.size();
		goalReached = true;

		break;
	case IDLE_r:
		break;
	}

	return 0;
}

uint8_t QuadFMS_t::FlyTrajectory(bool newPath){

	uint8_t status = 0;
	float resolutionSpeed;
	NavPoint wp;
	Position current, next;
	double distH,distV;

	if(newPath){
		trajectoryState = START_t;
	}

	switch(trajectoryState){

	case START_t:
		printf("executing trajectory resolution of size %d\n",FlightData->ResolutionPlan.size());
		FlightData->nextResolutionWP = 0;
		resolutionSpeed = FlightData->paramData->getValue("RES_SPEED");
		SetMode(_ACTIVE_); sleep(1);
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
		if(distH < captureH && distV < captureV){

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

uint8_t QuadFMS_t::FlyManeuver(bool newPlan){
	uint8_t status = 0;
	float resolutionSpeed = FlightData->paramData->getValue("RES_SPEED");
	static int countVel = 0;

	if(newPlan){
		maneuverState = START_m;
		countVel = 0;
	}

	countVel++;
	switch(maneuverState){

	case START_m:
		printf("executing maneuver resolution\n");
		SetMode(_ACTIVE_);
		SetSpeed(resolutionSpeed);
		maneuverState = GUIDE_m;
		usleep(100);
		//Removed break : Goto GUIDE state immediately after START without waiting for the next cycle
		//break;

	case GUIDE_m:
		if(Detector.trafficConflict){
			Resolver.ResolveTrafficConflictDAA();
			SetYaw(FlightData->maneuverHeading);
			SetVelocity(FlightData->maneuverVn,FlightData->maneuverVe,FlightData->maneuverVu);
		}
		else if(Detector.flightPlanDeviationConflict){
			Resolver.ResolveFlightPlanDeviation();
			if(countVel % 1000 == 0){
				SetYaw(FlightData->maneuverHeading);
				SetVelocity(FlightData->maneuverVn,FlightData->maneuverVe,FlightData->maneuverVu);
			}
		}
		else{
			printf("finished maneuver resolution\n");
			maneuverState = IDLE_m;
			Detector.clear();
			conflictSize = Detector.size();
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
	Position next;

	if(goalReached){
		next    = FlightData->MissionPlan.point(FlightData->nextMissionWP).position();
		resumeMission = true;
	}
	else{
		next = NextGoal;
		resumeMission = false;
	}

	double distH = current.distanceH(next);
	float speed  = FlightData->paramData->getValue("RES_SPEED");
	double ETA   = distH/speed;

	NavPoint wp1(current,0);
	NavPoint wp2(next,ETA);
	FlightData->ResolutionPlan.addNavPoint(wp1);
	FlightData->ResolutionPlan.addNavPoint(wp2);
	FlightData->nextResolutionWP = 0;
	planType      = TRAJECTORY;

}

uint8_t QuadFMS_t::TRACKING(Position target){


	// Get coordinates of object to track
	Position CurrentPos = FlightData->acState.positionLast();

	// Get tracking position
	// This is determined by parameters - distance and heading
	double heading = FlightData->paramData->getValue("TRK_HEADING");
	double distH   = FlightData->paramData->getValue("TRK_DISTH");
	double distV   = FlightData->paramData->getValue("TRK_DISTV");

	double distHx  = distH*sin(heading*M_PI/180); // Heading is measured from North
	double distHy  = distH*cos(heading*M_PI/180);

	double Kx_trk = 0.5;
	double Ky_trk = 0.5;
	double Kz_trk = 0.5;

	Vect3 delPos(distHx,distHy,distV);

	// Project from LatLonAlt to cartesian coordinates

	EuclideanProjection proj = Projection::createProjection(CurrentPos.mkAlt(0));
	Vect3 vecCP  =  proj.project(CurrentPos);
	Vect3 vecTP  =  proj.project(target);

	Vect3 vecTPf =  vecTP.Add(delPos);

	// Relative vector to object
	Vect3 Rel    = vecTPf.Sub(vecCP);

	// Compute velocity commands that will enure smooth tracking of object
	double dx = Rel.x;
	double dy = Rel.y;
	double dz = Rel.z;

	// Velocity is proportional to distance from object.
	double Vx = SaturateVelocity(Kx_trk * dx,2.0);
	double Vy = SaturateVelocity(Ky_trk * dy,2.0);
	double Vz = SaturateVelocity(Kz_trk * dz,2.0);

	//System.out.println(vecCP.toString());
	//System.out.println(vecTPf.toString());
	//System.out.format("dx,dy,dz = %f,%f,%f\n",dx,dy,dz);
	//System.out.format("Heading = %f, Velocities %1.3f, %1.3f, %1.3f\n",heading,Vx,Vy,Vz);

	// Vn, Ve, Vd
	SetVelocity(Vy,Vx,-Vz);

	double RefHeading = atan2(Vx,Vy)*180/M_PI;

	if(RefHeading < 0){
		RefHeading = 360 + RefHeading;
	}

	if(Rel.norm() > 0.3){
		SetYaw(RefHeading);
	}

	return 0;

}

double QuadFMS_t::SaturateVelocity(double V, double Vsat){
  if(std::abs(V) > Vsat){
    return sign(V)*Vsat;
  }
  else{
    return V;
  }
}


void QuadFMS_t::Reset(){
	fmsState = _idle_;
	planType = MISSION;
	conflictSize = 0;
	Detector.clear();
	landStarted = false;
}



