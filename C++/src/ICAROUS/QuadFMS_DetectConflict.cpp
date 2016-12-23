/**
 * Quad Flight Management System
 *
 * Core flight management functions for quadrotors
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

void QuadFMS_t::CheckGeofence(){
	Position CurrentPos = FlightData->acState.positionLast();
	Velocity CurrentVel = FlightData->acState.velocityLast();

	Conflict.keepin = false;
	Conflict.keepout = false;

	Plan CurrentFP;
	double elapsedTime;

	if(planType == MISSION){
		CurrentFP = FlightData->MissionPlan;
		elapsedTime = GetApproxElapsedPlanTime(CurrentFP,FlightData->nextMissionWP);
	}
	else{
		CurrentFP = FlightData->ResolutionPlan;
		elapsedTime = GetApproxElapsedPlanTime(CurrentFP,FlightData->nextResolutionWP);
	}
	// Check for geofence violation
	for(FlightData->fenceListIt = FlightData->fenceList.begin();
		FlightData->fenceListIt != FlightData->fenceList.end();
		++FlightData->fenceListIt){
		Geofence_t fence = *FlightData->fenceListIt;
		fence.CheckViolation(FlightData->acState,elapsedTime,CurrentFP);

		if(fence.GetConflictStatus() || fence.GetViolationStatus()){
			Conflict.AddConflict(fence);
			if(fence.GetType() == KEEP_IN){
				Conflict.keepin = true;
			}
			else{
				Conflict.keepout = true;
			}
		}
	}
}

void QuadFMS_t::CheckFlightPlanDeviation(){
	double allowedDev = FlightData->paramData->getValue("XTRK_DEV");
	double psi1,psi2,sgn;
	double bearing,dist;
	Plan CurrentFP;
	Position prevWP,nextWP,currentPos;

	if(planType == MISSION){
		CurrentFP = FlightData->MissionPlan;
		prevWP = CurrentFP.point(FlightData->nextMissionWP - 1).position();
		nextWP = CurrentFP.point(FlightData->nextMissionWP).position();
	}
	else{
		Conflict.flightPlanDeviation = false;
		return;
	}

	currentPos   = FlightData->acState.positionLast();
	psi1         = prevWP.track(nextWP) * 180/M_PI;
	psi2         = prevWP.track(currentPos) * 180/M_PI;
	sgn          = 0;

	if( (psi1 - psi2) >= 0){
	    sgn = 1;    // Vehicle left of the path
	}
	else if( (psi1 - psi2) <= 180){
	    sgn = -1;   // Vehicle right of the path
	}
	else if( (psi1 - psi2) < 0 ){
	    sgn = -1;   // Vehicle right of path
	}
	else if ( (psi1 - psi2) >= -180  ){
	    sgn = 1;    // Vehicle left of path
	}

	bearing = fabs(psi1 - psi2);
	dist    = prevWP.distanceH(currentPos);
	FlightData->crossTrackDeviation = sgn*dist*sin(bearing*M_PI/180);
	FlightData->crossTrackOffset    = dist*cos(bearing*M_PI/180);

	if(fabs(FlightData->crossTrackDeviation) > allowedDev){
		Conflict.flightPlanDeviation = true;
		//printf("Standoff conflict\n");
	}else if(fabs(FlightData->crossTrackDeviation) < (allowedDev)/2){
		Conflict.flightPlanDeviation = false;
	}


}

void QuadFMS_t::CheckTraffic(){

	time_t currentTime    = time(&currentTime);
	double daaTimeElapsed = difftime(currentTime,daaTimeStart);
	double elapsedTime    = difftime(currentTime,timeStart);

	Position so = FlightData->acState.positionLast();
	Velocity vo = FlightData->acState.velocityLast();

	DAA.reset();
	DAA.setOwnshipState("Ownship",so,vo,elapsedTime);

	for(unsigned int i=0;i<FlightData->trafficList.size();i++){
		Position si;
		Velocity vi;
		FlightData->GetTraffic(i,si,vi);
		char name[10];
		sprintf(name,"Traffic%d",i);
		DAA.addTrafficState(name,si,vi);
	}

	DAA.kinematicMultiBands(KMB);
	bool daaViolation = BandsRegion::isConflictBand(KMB.regionOfTrack(DAA.getOwnshipState().track()));

	if(daaViolation){
		Conflict.traffic = true;
		time(&daaTimeStart);
	}


	if(daaTimeElapsed > 10){
		if(!daaViolation){
			Conflict.traffic = false;
		}
	}
}




