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
#include "Units.h"

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

void QuadFMS_t::ComputeCrossTrackDev(Position pos,Plan fp,int nextWP,double stats[]){
		Plan CurrentPlan =  fp;

		Position PrevWP     = CurrentPlan.point(nextWP - 1).position();
		Position NextWP     = CurrentPlan.point(nextWP).position();
		double psi1         = PrevWP.track(NextWP) * 180/M_PI;
		double psi2         = PrevWP.track(pos) * 180/M_PI;
		double sgn          = 0;
		if( (psi1 - psi2) >= 0){
			sgn = 1;              // Vehicle left of the path
		}
		else if( (psi1 - psi2) <= 180){
			sgn = -1;             // Vehicle right of the path
		}
		else if( (psi1 - psi2) < 0 ){
			sgn = -1;             // Vehicle right of path
		}
		else if ( (psi1 - psi2) >= -180  ){
			sgn = 1;              // Vehicle left of path
		}
		double bearing = abs(psi1 - psi2);
		double dist = PrevWP.distanceH(pos);
		double crossTrackDeviation = sgn*dist*sin(bearing * M_PI/180);
		double crossTrackOffset    = dist*cos(bearing * M_PI/180);

		stats[0] = crossTrackDeviation;
		stats[1] = crossTrackOffset;
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

	double stats[2];
	ComputeCrossTrackDev(currentPos,CurrentFP,FlightData->nextMissionWP,stats);
	FlightData->crossTrackDeviation = stats[0];
	FlightData->crossTrackOffset    = stats[1];

	if(fabs(FlightData->crossTrackDeviation) > allowedDev){
		Conflict.flightPlanDeviation = true;
		//printf("Standoff conflict\n");
	}else if(fabs(FlightData->crossTrackDeviation) < (allowedDev)/2){
		Conflict.flightPlanDeviation = false;
	}
}


bool QuadFMS_t::CheckTurnConflict(double low,double high,double newHeading,double oldHeading){
	// Get direction of turn
	double psi   = newHeading - oldHeading;
	double psi_c = 360 - abs(psi);
	bool leftTurn = false;
	bool rightTurn = false;
	if(psi > 0){
		if(abs(psi) > abs(psi_c)){
			leftTurn = true;
		}
		else{
			rightTurn = true;
		}
	}else{
		if(abs(psi) > abs(psi_c)){
			rightTurn = true;
		}
		else{
			leftTurn = true;
		}
	}

	double A,B,X,Y,diff;
	if(rightTurn){
		diff = oldHeading;
		A = oldHeading - diff;
		B = newHeading - diff;
		X = low - diff;
		Y = high - diff;

		if(B < 0){
			B = 360 + B;
		}

		if(X < 0){
			X = 360 + X;
		}

		if(Y < 0){
			Y = 360 + Y;
		}

		if(A < X && B > Y){
			return true;
		}
	}else{
		diff = 360 - oldHeading;
		A    = oldHeading + diff;
		B    = newHeading + diff;
		X = low + diff;
		Y = high + diff;

		if(B > 360){
			B = B - 360;
		}

		if(X > 360){
			X = X - 360;
		}

		if(Y > 360){
			Y = Y - 360;
		}

		if(A > Y && B < X){
			return true;
		}
	}

	return false;
}

void QuadFMS_t::CheckTraffic(){


	//TODO: Set latest daa parameters
	double radius = FlightData->paramData->getValue("CYL_RADIUS");
	double height = FlightData->paramData->getValue("CYL_HEIGHT");
	double alertTime = FlightData->paramData->getValue("ALERT_TIME");
	double earlyAlertTime = FlightData->paramData->getValue("EARLY_ALERT_TIME");
	CDCylinder cd = CDCylinder(radius,"m",height,"m");
	AlertThresholds alertor(&cd,alertTime,earlyAlertTime,BandsRegion::NEAR);
	DAA.parameters.alertor.setLevel(1,alertor);
	DAA.parameters.setLookaheadTime(FlightData->paramData->getValue("DAA_LOOKAHEAD"));

	time_t currentTime    = time(&currentTime);
	double daaTimeElapsed = difftime(currentTime,daaTimeStart);
	double elapsedTime    = difftime(currentTime,timeStart);

	Position so = FlightData->acState.positionLast();
	Velocity vo = FlightData->acState.velocityLast();

	DAA.reset();
	DAA.setOwnshipState("Ownship",so,vo,elapsedTime);

	double dist2traffic = MAXDOUBLE;
	for(unsigned int i=0;i<FlightData->trafficList.size();i++){
		Position si;
		Velocity vi;
		FlightData->GetTraffic(i,si,vi);
		char name[10];
		sprintf(name,"Traffic%d",i);
		DAA.addTrafficState(name,si,vi);
		double dist = so.distanceH(si);
		if(dist < dist2traffic){
			dist2traffic = dist;
		}
	}

	DAA.kinematicMultiBands(KMB);
	bool daaViolation = BandsRegion::isConflictBand(KMB.regionOfTrack(DAA.getOwnshipState().track()));

	if(daaViolation){
		Conflict.traffic = true;
		time(&daaTimeStart);
	}


	if(daaTimeElapsed > 5){
		if(!daaViolation){
			Conflict.traffic = returnPathConflict;
		}
	}

	// Construct kinematic bands message to send to ground station
	mavlink_kinematic_bands_t msg;

	msg.numBands = KMB.trackLength();

	for(int i=0;i<msg.numBands;++i){
		Interval iv = KMB.track(i,"deg");
		int type = 0;
		if( KMB.trackRegion(i) == BandsRegion::NONE){
			type = 0;
		}
		else if(KMB.trackRegion(i) == BandsRegion::NEAR){
			type = 1;
		}

		if(i==0){
			msg.type1 =  type;
			msg.min1 = (float) iv.low;
			msg.max1 = (float) iv.up;
		}else if(i==1){
			msg.type2 =  type;
			msg.min2 = (float) iv.low;
			msg.max2 = (float) iv.up;
		}else if(i==2){
			msg.type3 =  type;
			msg.min3 = (float) iv.low;
			msg.max3 = (float) iv.up;
		}else if(i==3){
			msg.type4 =  type;
			msg.min4 = (float) iv.low;
			msg.max4 = (float) iv.up;
		}else{
			msg.type5 =  type;
			msg.min5 = (float) iv.low;
			msg.max5 = (float) iv.up;
		}
	}

	if(msg.numBands > 0){
		if(dist2traffic < 20){
			FlightData->RcvdMessages->AddKinematicBands(msg);
		}
		else if(msg.numBands == 1 && msg.type1 == 0){
			// Don't send if we only have one type 0 band
			// This is to save bandwidth
		}else{
			FlightData->RcvdMessages->AddKinematicBands(msg);
		}
	}
}




