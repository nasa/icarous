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

void QuadFMS_t::ResolveFlightPlanDeviation(){

	double xtrkDevGain ;
	double resolutionSpeed;
	double allowedDev;
	double Vs,Vf,V,sgn;
	double Trk;
	Position prevWP,nextWP,currentPos,cp;
	Plan currentFP;

	xtrkDevGain     = FlightData->paramData->getValue("XTRK_GAIN");
	resolutionSpeed = FlightData->paramData->getValue("RES_SPEED");
	allowedDev      = FlightData->paramData->getValue("XTRK_DEV");

	currentPos = FlightData->acState.positionLast();

	currentFP = FlightData->MissionPlan;
	prevWP = currentFP.point(FlightData->nextMissionWP - 1).position();
	nextWP = currentFP.point(FlightData->nextMissionWP).position();

	if(xtrkDevGain < 0){
		xtrkDevGain = -xtrkDevGain;
	}

	if(fabs(FlightData->crossTrackDeviation) <= 3*allowedDev){
		Vs = xtrkDevGain*FlightData->crossTrackDeviation;
		V  = resolutionSpeed;

		if(Vs >= 0){
			sgn = 1;
		}
		else{
			sgn = -1;
		}

		if(pow(fabs(Vs),2) >= pow(V,2)){
			Vs = V*sgn;
		}

		Vf = sqrt(pow(V,2) - pow(Vs,2));

		Trk = prevWP.track(nextWP);
		FlightData->maneuverVn = Vf*cos(Trk) - Vs*sin(Trk);
		FlightData->maneuverVe = Vf*sin(Trk) + Vs*cos(Trk);
		FlightData->maneuverVu = 0;

		FlightData->maneuverHeading = atan2(FlightData->maneuverVe,FlightData->maneuverVn)*180/M_PI;
		if(FlightData->maneuverHeading < 0){
			FlightData->maneuverHeading = 360 + FlightData->maneuverHeading;
		}

		planType = MANEUVER;

	}
	else{

		Position cp = GetPointOnPlan(FlightData->crossTrackOffset,currentFP,FlightData->nextMissionWP);
		double distance = currentPos.distanceH(cp);
		double ETA      = distance/resolutionSpeed;

		NavPoint wp1(currentPos,0);
		NavPoint wp2(cp,ETA);
		FlightData->ResolutionPlan.clear();
		FlightData->ResolutionPlan.add(wp1);
		FlightData->ResolutionPlan.add(wp2);

		std::cout<<FlightData->ResolutionPlan.toString()<<std::endl;
		planType      = TRAJECTORY;
		resumeMission = false;
		goalReached   = true;
	}
}

void QuadFMS_t::ResolveKeepInConflict(){
	Geofence_t fence = Conflict.GetKeepInConflict();
	NavPoint wp(fence.GetRecoveryPoint(),0);
	std::cout<<wp.position().toStringUnits("degree","degree","m")<<std::endl;
	NavPoint next_wp = FlightData->MissionPlan.point(FlightData->nextMissionWP);
	FlightData->ResolutionPlan.clear();
	FlightData->ResolutionPlan.add(wp);
	FlightData->nextResolutionWP = 0;

	if(fence.CheckWPFeasibility(wp.position(),next_wp.position())){
		FlightData->nextMissionWP++;
	}

	planType        = TRAJECTORY;
	resumeMission   = false;
	goalReached     = true;
	return;
}

void QuadFMS_t::ResolveKeepOutConflict_Astar(){

	double gridsize          = FlightData->paramData->getValue("GRIDSIZE");
	double buffer            = FlightData->paramData->getValue("BUFFER");
	double lookahead         = FlightData->paramData->getValue("LOOKAHEAD");
	double resolutionSpeed   = FlightData->paramData->getValue("RES_SPEED");
	double maxAlt            = FlightData->paramData->getValue("MAX_CEILING");

	// Reroute flight plan
	SetMode(GUIDED); // Set mode to guided for quadrotor to hover before replanning

	Position currentPos = FlightData->acState.positionLast();
	Velocity currentVel = FlightData->acState.velocityLast();

	double elapsedTime;
	double altFence;
	double minTime = MAXDOUBLE;
	double maxTime = 0;

	Plan currentFP;
	Position prevWP;
	Position nextWP;
	Position start = currentPos;

	if(planType == MISSION){
		currentFP = FlightData->MissionPlan;
		elapsedTime = GetApproxElapsedPlanTime(currentFP,FlightData->nextMissionWP);
		prevWP = currentFP.point(FlightData->nextMissionWP - 1).position();
		nextWP = currentFP.point(FlightData->nextMissionWP).position();
	}
	else if(planType == TRAJECTORY){
		currentFP = FlightData->ResolutionPlan;
		elapsedTime = GetApproxElapsedPlanTime(currentFP,FlightData->nextResolutionWP);
		prevWP = currentFP.point(FlightData->nextResolutionWP - 1).position();
		nextWP = currentFP.point(FlightData->nextResolutionWP).position();
	}


	std::list<Geofence_t>::iterator it;
	for(it=Conflict.keepOutGeofence.begin();
			it!=Conflict.keepOutGeofence.end();++it){
		double entrytime, exittime;
		it->GetEntryExitTime(entrytime,exittime);

		if(entrytime <= minTime){
			minTime  = entrytime;
			altFence = it->GetCeiling();

			if(it->GetViolationStatus()){
				start = it->GetRecoveryPoint();
			}
		}

		if(exittime >= maxTime){
			maxTime = exittime;
		}

		if(it->GetViolationStatus()){
			start = it->GetRecoveryPoint();
		}
	}

	minTime = minTime - lookahead;
	maxTime = maxTime + lookahead;

	if (minTime < elapsedTime) {
		minTime = elapsedTime + 0.1;
	}

	if (maxTime > currentFP.getLastTime()) {
		maxTime = currentFP.getLastTime() - 0.1;
	}

	if(planType == MISSION){
		FlightData->nextMissionWP = FlightData->MissionPlan.getSegment(maxTime) + 1;
	}

	Plan conflictFP = PlanUtil::cutDown(currentFP,minTime,maxTime);
	Position goal = conflictFP.getLastPoint().position();

	BoundingRectangle BR;
	for(it=FlightData->fenceList.begin();
			it!=FlightData->fenceList.end();++it){
		if(it->GetType() == KEEP_IN){
			maxAlt = it->GetPoly3D().getTop();
			for(int i=0;i<it->GetSize();++i){
				BR.add(it->GetPoly().getVertex(i));
			}
			break;
		}
	}

	NavPoint initpos(start,0);
	DensityGrid DG(BR,initpos,goal,(int)buffer,gridsize,true);
	DG.snapToStart();
	DG.setWeights(5.0);

	for(it=FlightData->fenceList.begin();
			it!=FlightData->fenceList.end();++it){
		if(it->GetType() == KEEP_OUT){
			DG.setWeightsInside(it->GetPoly(),100.0);
		}
	}

	DensityGridAStarSearch DGAstar;
	std::vector<std::pair<int,int>> GridPath = DGAstar.optimalPath(DG);
	std::vector<std::pair<int,int>>::iterator gpit;

	std::pair<int,int> p3(103,63);
	Position pos2 = DG.getPosition(p3);

	Plan ResolutionPlan1;
	//Create a plan out of the grid points
	if(!GridPath.empty()){
		std::list<Position> PlanPosition;
		double currHeading = 0.0;
		double nextHeading = 0.0;

		// Reduce the waypoints based on heading
		PlanPosition.push_back(start);
		double startAlt = start.alt();

		for(gpit = GridPath.begin(); gpit != GridPath.end(); ++gpit){
			Position pos1 = DG.getPosition(*gpit);

			if(gpit == GridPath.begin()){
				++gpit;
				Position pos2 = DG.getPosition(*gpit);--gpit;
				currHeading = pos1.track(pos2);
				continue;
			}

			if( ++gpit == GridPath.end() ){
				--gpit;
				PlanPosition.push_back(pos1.mkAlt(startAlt));
				break;
			}
			else{
				Position pos2 = DG.getPosition(*gpit);--gpit;
				nextHeading = pos1.track(pos2);
				if(fabs(nextHeading - currHeading) > 0.01){
					PlanPosition.push_back(pos1.mkAlt(startAlt));
					currHeading = nextHeading;
				}
			}

		}
		PlanPosition.push_back(goal);


		double ETA = 0.0;
		NavPoint wp0(PlanPosition.front(),0);
		ResolutionPlan1.add(wp0);

		int count = 0;
		std::list<Position>::iterator it;
		for(it = PlanPosition.begin();it != PlanPosition.end();++it){
			Position pos = *it;
			if(count == 0){
				ETA = 0;
			}
			else{
				Position prevWP = ResolutionPlan1.point(count-1).position();
				double distH    = pos.distanceH(prevWP);
				ETA             = ETA + distH/resolutionSpeed;
			}
			NavPoint np(pos,ETA);
			ResolutionPlan1.add(np);
			count++;
		}
	}

	Plan ResolutionPlan2 = ComputeGoAbovePlan(start,goal,altFence,resolutionSpeed);

	double length1 = ResolutionPlan1.pathDistance();
	double length2 = ResolutionPlan2.pathDistance();

	if( (altFence >= maxAlt) ){
		length2 = MAXDOUBLE;
	}

	FlightData->ResolutionPlan.clear();
	if(length1 < length2){
		FlightData->ResolutionPlan = ResolutionPlan1;
	}else{
		FlightData->ResolutionPlan = ResolutionPlan2;
	}

	planType        = TRAJECTORY;
	resumeMission   = false;
	goalReached     = true;
	return;

}

void QuadFMS_t::ResolveKeepOutConflict_RRT(){

	double resolutionSpeed   = FlightData->paramData->getValue("RES_SPEED");
	double maxAlt            = FlightData->paramData->getValue("MAX_CEILING");

	// Reroute flight plan
	SetMode(GUIDED); // Set mode to guided for quadrotor to hover before replanning

	std::vector<Position> TrafficPos;
	std::vector<Velocity> TrafficVel;

	Position currentPos = FlightData->acState.positionLast();
	Velocity currentVel = FlightData->acState.velocityLast();

	double altFence;
	double minTime = MAXDOUBLE;
	double maxTime = 0;

	Plan currentFP;
	Position prevWP;
	Position nextWP;
	Position start = currentPos;

	if(planType == MISSION){
		currentFP = FlightData->MissionPlan;
		prevWP = currentFP.point(FlightData->nextMissionWP - 1).position();
		nextWP = currentFP.point(FlightData->nextMissionWP).position();
	}
	else if(planType == TRAJECTORY){
		currentFP = FlightData->ResolutionPlan;
		prevWP = currentFP.point(FlightData->nextResolutionWP - 1).position();
		nextWP = currentFP.point(FlightData->nextResolutionWP).position();
	}

	std::list<Geofence_t>::iterator it;
	for(it=Conflict.keepOutGeofence.begin();
		it!=Conflict.keepOutGeofence.end();++it){
		double entrytime, exittime;
		it->GetEntryExitTime(entrytime,exittime);

		if(entrytime <= minTime){
			minTime  = entrytime;
			altFence = it->GetCeiling();

			if(it->GetViolationStatus()){
				start = it->GetRecoveryPoint();
			}
		}

		if(exittime >= maxTime){
			maxTime = exittime;
		}
	}

	if(planType == MISSION){
		FlightData->nextMissionWP = FlightData->MissionPlan.getSegment(maxTime) + 1;
	}

	Plan conflictFP = PlanUtil::cutDown(currentFP,minTime,maxTime);
	Position goal = conflictFP.getLastPoint().position();

	int Nsteps = 1000;
	int Tstep  = 5;
	double dT  = 1;
	RRT_t RRT(FlightData->fenceList,start,currentVel,TrafficPos,TrafficVel,Tstep,dT);
	RRT.SetGoal(goal);

	int i;
	for(i=0;i<Nsteps;i++){
		RRT.RRTStep(i);
		if(RRT.CheckGoal()){
			printf("Goal found\n");
			break;
		}
	}

	printf("iteration count = %d\n",i);

	goalReached = RRT.goalreached;
	if(!goalReached){
		NextGoal = goal;
	}

	Plan ResolutionPlan1 = RRT.GetPlan();
	Plan ResolutionPlan2 = ComputeGoAbovePlan(start,goal,altFence,resolutionSpeed);

	double length1 = ResolutionPlan1.pathDistance();
	double length2 = ResolutionPlan2.pathDistance();

	if( (altFence >= maxAlt) ){
		length2 = MAXDOUBLE;
	}

	FlightData->ResolutionPlan.clear();
	if(length1 < length2){
		FlightData->ResolutionPlan = ResolutionPlan1;
	}else{
		FlightData->ResolutionPlan = ResolutionPlan2;
	}

	planType        = TRAJECTORY;
	resumeMission   = false;
	return;

}

Plan QuadFMS_t::ComputeGoAbovePlan(Position start,Position goal,double altFence,double rSpeed){
	// Compute go above plan
	Plan ResolutionPlan2;
	double ETA = 0;
	double distH,distV;

	NavPoint nvpt1(start,ETA);
	ResolutionPlan2.add(nvpt1);

	// Second waypoint directly above WP1
	Position wp2 = start.mkAlt(altFence+1);
	distV = wp2.distanceV(start);
	ETA = ETA + distV/rSpeed;
	NavPoint nvpt2(wp2,ETA);
	ResolutionPlan2.add(nvpt2);

	// Third waypoint directly above exit point
	Position wp3 = goal.mkAlt(altFence+1);
	distH = wp3.distanceH(wp2);
	ETA = ETA + distH/rSpeed;
	NavPoint nvpt3(wp3,ETA);
	ResolutionPlan2.add(nvpt3);

	// Final waypoint
	distV = goal.distanceH(wp3);
	ETA = ETA + distV/rSpeed;
	NavPoint nvpt4(goal,ETA);
	ResolutionPlan2.add(nvpt4);

	return ResolutionPlan2;
}

Position QuadFMS_t::GetPointOnPlan(double offset,Plan fp,int next){

	Position nextWP       = fp.point(next).position();
	Position prevWP       = fp.point(next-1).position();
	double headingNextWP  = prevWP.track(nextWP);;
	double dn             = offset*cos(headingNextWP);
	double de             = offset*sin(headingNextWP);
	Position cp           = prevWP.linearEst(dn, de);

	if(cp.alt() <= 0){
		cp = cp.mkAlt(nextWP.alt());
	}

	return cp;

}

void QuadFMS_t::ResolveTrafficConflictDAA(){
	// Track based resolutions
	//TODO: add eps to preferred heading
	Position currentPos = FlightData->acState.positionLast();
	Velocity currentVel = FlightData->acState.velocityLast();
	double speed = FlightData->speed;


	if(abs(currentVel.groundSpeed("m/s") - speed) > 0.2){
		currentVel = lastVelocity;
		// Note: The speed can drop to 0 because of setting mode to guided.
		// We need to avoid this transience. Hence, store the last know velocity
		// whose speed is > 0.5
	}

	returnPathConflict = true;
	double resolutionSpeed = FlightData->speed;

	double crossStats[2];
	ComputeCrossTrackDev(currentPos, FlightData->MissionPlan, FlightData->nextMissionWP,crossStats);
	Position goal = GetPointOnPlan(crossStats[1], FlightData->MissionPlan,FlightData->nextMissionWP);

	double currentHeading = currentVel.trk();
	double nextHeading = currentPos.track(goal);
	Velocity nextVel   = Velocity::makeTrkGsVs(nextHeading,"radians",resolutionSpeed,"m/s",0,"m/s");
	double alertTime   = currentPos.distanceH(goal)/resolutionSpeed;
	//Use new alert time if it is greater than existing alert time
	alertTime = Util::max(alertTime,alertTime0);

	AlertThresholds alertor = DAAresolution.parameters.alertor.getLevel(1);
	alertor.setAlertingTime(alertTime);
	alertor.setEarlyAlertingTime(alertTime);
	DAAresolution.parameters.alertor.setLevel(1,alertor);

	DAAresolution.setOwnshipState("Ownship", currentPos, currentVel, FlightData->acTime);
	std::list<Object_t>::iterator it;
	int count = 0;
	for(it = FlightData->trafficList.begin();it != FlightData->trafficList.end();it++){
		Position tPos = Position::makeLatLonAlt(it->x,"degree",it->y,"degree",it->z,"m");
		Velocity tVel = Velocity::makeVxyz(it->vy,it->vx,"m/s",it->vz,"m/s");
		char name[10];
		sprintf(name,"ResTraffic%d",count);count++;
		DAAresolution.addTrafficState(name, tPos, tVel);
	}

	KinematicMultiBands KMB;
	DAAresolution.kinematicMultiBands(KMB);
	returnPathConflict  = BandsRegion::isConflictBand(KMB.regionOfTrack(nextHeading));

	bool prefDirection = KMB.preferredTrackDirection();
	double prefHeading    = KMB.trackResolution(prefDirection);

	if(prefDirection){
		prefHeading = prefHeading + 2*M_PI/180;
		if(prefHeading > M_PI){
			prefHeading = prefHeading - 2*M_PI;
		}
	}else{
		prefHeading = prefHeading - 2*M_PI/180;
		if(prefHeading < -M_PI){
			prefHeading = 2*M_PI + prefHeading;
		}
	}

	count = 0;
	for(int i=0;i<KMB.trackLength();++i){
		Interval iv = KMB.track(i, "deg"); // i-th band region
		double lower_trk = iv.low; // [deg]
		double upper_trk = iv.up; // [deg]
		if (KMB.trackRegion(i) == BandsRegion::NONE) {
			bool val = CheckTurnConflict(lower_trk, upper_trk, nextHeading*180/M_PI,  currentHeading*180/M_PI);
			if(val){
				count++;
			}
		}
	}

	if(count>0){
		returnPathConflict = true;
	}

	if(!isnanf(prefHeading)){
		FlightData->maneuverVn = resolutionSpeed * cos(prefHeading);
		FlightData->maneuverVe = resolutionSpeed * sin(prefHeading);
		FlightData->maneuverHeading = atan2(FlightData->maneuverVe,FlightData->maneuverVn)*180/M_PI;
	}

	if(FlightData->maneuverHeading < 0){
		FlightData->maneuverHeading = 360 + FlightData->maneuverHeading;
	}

	planType = MANEUVER;

	if(debugDAA){
		debugIO<<"**********"<<FlightData->acTime<<"**********"<<std::endl;
		debugIO<<KMB.outputString()<<std::endl;
		debugIO<<"Vn,Ve"<<FlightData->maneuverVn<<FlightData->maneuverVe<<std::endl;
		debugIO<<"Return path conflict:"<<returnPathConflict<<std::endl;
	}

}

void QuadFMS_t::ResolveTrafficConflictRRT(){

	// Reroute flight plan
	SetMode(GUIDED); // Set mode to guided for quadrotor to hover before replanning

	std::vector<Position> TrafficPos;
	std::vector<Velocity> TrafficVel;

	Position currentPos = FlightData->acState.positionLast();
	Velocity currentVel = FlightData->acState.velocityLast();

	std::list<Object_t>::iterator it;
	for(it=FlightData->trafficList.begin();
		it!=FlightData->trafficList.end();++it){
		Position tPos = Position::makeLatLonAlt(it->x,"degree",it->y,"degree",it->z,"m");
		Velocity tVel = Velocity::makeVxyz(it->vy,it->vx,"m/s",it->vz,"m/s");
		TrafficPos.push_back(tPos);
		TrafficVel.push_back(tVel);
	}

	Plan currentFP;
	Position prevWP;
	Position nextWP;
	Position start = currentPos;


	if(planType == MISSION){
		currentFP = FlightData->MissionPlan;
		prevWP = currentFP.point(FlightData->nextMissionWP - 1).position();
		nextWP = currentFP.point(FlightData->nextMissionWP).position();
	}
	else if(planType == TRAJECTORY){
		currentFP = FlightData->ResolutionPlan;
		prevWP = currentFP.point(FlightData->nextResolutionWP - 1).position();
		nextWP = currentFP.point(FlightData->nextResolutionWP).position();
	}

	Position goal = nextWP;

	int Nsteps = 500;
	int Tstep  = 5;
	double dT  = 1;
	RRT_t RRT(FlightData->fenceList,start,currentVel,TrafficPos,TrafficVel,Tstep,dT);
	RRT.SetGoal(goal);

	for(int i=0;i<Nsteps;i++){
		RRT.RRTStep(i);
		if(RRT.CheckGoal()){
			//printf("Goal found\n");
			break;
		}
	}

	goalReached = RRT.goalreached;
	if(!goalReached){
		NextGoal = goal;
	}

	FlightData->ResolutionPlan.clear();
	FlightData->ResolutionPlan = RRT.GetPlan();
	planType        = TRAJECTORY;
	resumeMission   = false;
	return;
}


