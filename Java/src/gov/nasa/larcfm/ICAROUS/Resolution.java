package gov.nasa.larcfm.ICAROUS;

import java.util.*;
import java.lang.*;
import java.io.*;
import com.MAVLink.common.*;
import com.MAVLink.enums.*;

import gov.nasa.larcfm.Util.*;
import gov.nasa.larcfm.ACCoRD.*;
import gov.nasa.larcfm.ICAROUS.FlightManagementSystem.ARDUPILOT_MODES;
import gov.nasa.larcfm.ICAROUS.GeoFence.FENCE_TYPE;
import gov.nasa.larcfm.ICAROUS.QuadFMS.plan_type_t;

public class Resolution {

	private QuadFMS FMS;
	private AircraftData FlightData;
	private double resolutionSpeed;
	private double gridsize;
	private double buffer;
	private double lookahead;
	private double allowedDev;
	private double xtrkDevGain;
	
	public Resolution(QuadFMS fms){
		FMS = fms;
		FlightData = FMS.FlightData;
		resolutionSpeed = 1.0;
	}

	public void ResolveKeepInConflict(){
		
		Plan CurrentPlan;
		int nextWPind;
		CurrentPlan =  FlightData.MissionPlan;	
		nextWPind = FlightData.nextMissionWP;
				
		GeoFence GF = FMS.Detector.keepInFence;
		NavPoint wp = null;		
		wp = new NavPoint(GF.RecoveryPoint,0);
		
		FMS.FlightData.ResolutionPlan.clear();
		FMS.FlightData.nextResolutionWP = 0;
		FMS.FlightData.ResolutionPlan.add(wp);
		
		NavPoint nextWP = CurrentPlan.point(nextWPind);
		if(!GF.CheckWaypointFeasibility(wp.position(),nextWP.position())){
			FlightData.nextMissionWP++;
		}
		
		FMS.GoalReached = true;
		FMS.planType = plan_type_t.TRAJECTORY;
		FMS.resumeMission = false;

		return;
	}

	public void ResolveKeepOutConflictAstar(){
		gridsize          = FlightData.pData.getValue("GRIDSIZE");
		buffer            = FlightData.pData.getValue("BUFFER");
		lookahead         = FlightData.pData.getValue("LOOKAHEAD");
		resolutionSpeed   = FlightData.pData.getValue("RES_SPEED");

		// Reroute flight plan
		FMS.SetMode(ARDUPILOT_MODES.GUIDED); // Set mode to guided for quadrotor to hover before replanning

		Plan CurrentFP;
		double currentTime;
		double altfence = 100;
		double maxalt = 0.0;
		int nextWP;

		if(FMS.planType == plan_type_t.MISSION){
			CurrentFP = FlightData.MissionPlan;
			nextWP    = FlightData.nextMissionWP;
			currentTime = FMS.getApproxElapsedPlanTime(CurrentFP,nextWP);
		}
		else{
			CurrentFP = FlightData.ResolutionPlan;
			nextWP    = FlightData.nextResolutionWP;
			currentTime = FMS.getApproxElapsedPlanTime(CurrentFP,nextWP);
		}

		double minTime = Double.MAX_VALUE;
		double maxTime = 0.0;

		ArrayList<PolyPath> LPP = new ArrayList<PolyPath>(); // List of keep out fences
		ArrayList<PolyPath> CPP = new ArrayList<PolyPath>(); // List of keep in fences
		WeatherUtil WU = new WeatherUtil();

		Position RecoveryPoint = null;
		boolean violation = false;

		// Get conflict start and end time
		

		GeoFence KF = (GeoFence) FMS.Detector.keepOutFence;
		if(KF.entryTime <= minTime){
			minTime = KF.entryTime;
			altfence = KF.ceiling;
		}

		if(KF.exitTime >= maxTime){
			maxTime = KF.exitTime;
		}

		LPP.add(KF.geoPolyPath);

		if(KF.violation){
			RecoveryPoint = KF.RecoveryPoint;
			violation = true;
		}



		for(int i=0;i<FlightData.fenceList.size();i++){
			GeoFence GF = FlightData.fenceList.get(i);
			if(GF.fType == FENCE_TYPE.KEEP_IN){
				CPP.add(GF.geoPolyPath);
				maxalt = GF.ceiling;
			}
		}

		minTime = minTime - lookahead;
		maxTime = maxTime + lookahead;

		if(minTime < currentTime){
			minTime = currentTime+0.1;
		}

		if(maxTime > CurrentFP.getLastTime()){
			maxTime = CurrentFP.getLastTime() - 0.1;
		}

		if(FMS.planType == plan_type_t.MISSION){
			FlightData.nextMissionWP = CurrentFP.getSegment(maxTime)+1;
		}

		// Get flight plan between start time and end time (with a 3 second buffer on both sides)
		Plan ConflictFP = PlanUtil.cutDown(CurrentFP,minTime,maxTime);

		// Create a bounding box based on containment geofence
		BoundingRectangle BR = new BoundingRectangle();

		SimplePoly CF = null;
		for(int i=0;i<FlightData.fenceList.size();i++){
			GeoFence GF = FlightData.fenceList.get(i);
			if(GF.fType == FENCE_TYPE.KEEP_IN){
				CF = FlightData.fenceList.get(i).geoPolyLLA;
			}
		}		

		for(int i=0;i<CF.size();i++){
			BR.add(CF.getVertex(i));
		}	

		// Get start and end positions based on conflicted parts of the original flight plan	
		NavPoint start = ConflictFP.point(0);
		Position end   = ConflictFP.getLastPoint().position();

		if(violation){
			start = new NavPoint(RecoveryPoint,0);
		}

		// Instantiate a grid to search over
		DensityGrid dg = new DensityGrid(BR,start,end,(int)buffer,gridsize,true);
		dg.snapToStart();

		// Set weights for the search space and obstacles
		dg.setWeights(5.0);

		for(int i=0;i<FlightData.fenceList.size();i++){	    
			GeoFence GF = FlightData.fenceList.get(i);
			if(GF.fType == FENCE_TYPE.KEEP_OUT){
				
				SimplePoly expfence = GF.geoPolyLLA2;
				dg.setWeightsInside(expfence,100.0);
			}
		}

		// Perform A* seartch
		DensityGridAStarSearch DGAstar = new DensityGridAStarSearch();
		List<Pair <Integer,Integer>> GridPath = DGAstar.optimalPath(dg);
		Plan ResolutionPlan1 = new Plan(); // Go around plan
		Plan ResolutionPlan2 = new Plan(); // Go above plan
		double pathLength1 = Double.MAX_VALUE;
		double pathLength2 = Double.MAX_VALUE;

		if(GridPath != null){	    		
			List<Position> PlanPosition = new ArrayList<Position>();
			double currHeading = 0.0;
			double nextHeading = 0.0;

			// Reduce number of waypoints based on heading
			PlanPosition.add(start.position());
			double startAlt = start.position().alt();
			PlanPosition.add(dg.getPosition(GridPath.get(0)).mkAlt(startAlt));
			currHeading = dg.getPosition(GridPath.get(0)).track(dg.getPosition(GridPath.get(1)));

			for(int i=1;i<GridPath.size();i++){
				Position pos1 = dg.getPosition(GridPath.get(i));	    	    	    
				if(i==GridPath.size()-1){
					PlanPosition.add(pos1.mkAlt(startAlt));
					break;
				}
				else{
					Position pos2 = dg.getPosition(GridPath.get(i+1));
					nextHeading = pos1.track(pos2);		
					if(Math.abs(nextHeading - currHeading) > 0.01){		    		
						PlanPosition.add(pos1.mkAlt(startAlt));
						currHeading = nextHeading;
					}
				}	    
			}
			PlanPosition.add(end);

			// Create new flight plan based on waypoints
			ResolutionPlan1.clear();
			FlightData.nextResolutionWP = 0;
			double ETA   = 0.0;
			ResolutionPlan1.add(new NavPoint(PlanPosition.get(0),ETA));
			for(int i=1;i<PlanPosition.size();i++){
				Position pos = PlanPosition.get(i);
				double distance = pos.distanceH(PlanPosition.get(i-1));
				ETA      = ETA + distance/resolutionSpeed;

				ResolutionPlan1.add(new NavPoint(pos,ETA));
			}
			pathLength1 = ResolutionPlan1.pathDistance();
		}

		//Compute go above plan
		ResolutionPlan2 = ComputeGoAbovePlan(start.position(),end,altfence,resolutionSpeed);

		pathLength2 = ResolutionPlan2.pathDistance();

		if( (maxalt - altfence) < 3 ){
			pathLength2 = Double.MAX_VALUE;
		}

		if(pathLength1 < pathLength2){
			FlightData.ResolutionPlan = ResolutionPlan1;
			FMS.log.addWarning("MSG: Using go around plan");		
		}
		else{
			FlightData.ResolutionPlan = ResolutionPlan2;
			FMS.log.addWarning("MSG: Using go above plan");		
		}
		
		
		FMS.GoalReached = true;
		FMS.planType = plan_type_t.TRAJECTORY;
		FMS.resumeMission = false;
	}
	
	public void ResolveKeepOutConflictRRT(){
		double resolutionSpeed   = FlightData.pData.getValue("RES_SPEED");
		double maxAlt            = FlightData.pData.getValue("MAX_CEILING");

		// Reroute flight plan
		ArrayList<Position> TrafficPos = new ArrayList<Position>();
		ArrayList<Velocity> TrafficVel = new ArrayList<Velocity>();

		Position currentPos = FlightData.acState.positionLast();
		Velocity currentVel = FlightData.acState.velocityLast();

		double elapsedTime;
		double altFence = 0;
		double minTime = Double.MAX_VALUE;
		double maxTime = 0;

		Plan currentFP = null;
		Position prevWP = null;
		Position nextWP = null;
		Position start = currentPos;

		if(FMS.planType == plan_type_t.MISSION){
			currentFP = FlightData.MissionPlan;
			elapsedTime = FMS.getApproxElapsedPlanTime(currentFP,FlightData.nextMissionWP);
			prevWP = currentFP.point(FlightData.nextMissionWP - 1).position();
			nextWP = currentFP.point(FlightData.nextMissionWP).position();
		}
		else if(FMS.planType == plan_type_t.TRAJECTORY){
			currentFP = FlightData.ResolutionPlan;
			elapsedTime = FMS.getApproxElapsedPlanTime(currentFP,FlightData.nextResolutionWP);
			prevWP = currentFP.point(FlightData.nextResolutionWP - 1).position();
			nextWP = currentFP.point(FlightData.nextResolutionWP).position();
		}

		
		GeoFence GF = FMS.Detector.keepOutFence;
		double entrytime = GF.entryTime;
		double exittime = GF.exitTime;

		if(entrytime <= minTime){
			minTime  = entrytime;
			altFence = GF.ceiling;

			if(GF.violation){
				start = GF.RecoveryPoint;
			}
		}

		if(exittime >= maxTime){
			maxTime = exittime;
		}
				
		maxTime = maxTime + 2; // add a couple seconds to the maxTime 
		if(FlightData.MissionPlan.getLastTime() < maxTime){
			maxTime = FlightData.MissionPlan.getLastTime();
		}
		
		
		if(FMS.planType == plan_type_t.MISSION){
			FlightData.nextMissionWP = FlightData.MissionPlan.getSegment(maxTime) + 1;
		}

		Plan conflictFP = PlanUtil.cutDown(currentFP,minTime,maxTime);
		Position goal = conflictFP.getLastPoint().position();

		int Nsteps = 1000;
		int Tstep  = 5;
		double dT  = 1;
		RRT rrt = new RRT(FlightData.fenceList,start,currentVel,TrafficPos,TrafficVel,Tstep,dT);
		rrt.SetGoal(goal);

		int i;
		for(i=0;i<Nsteps;i++){
			rrt.RRTStep(i);
			if(rrt.CheckGoal()){
				break;
			}
		}

		
		System.out.format("iteration count = %d\n",i);
		FMS.GoalReached = rrt.goalreached;
		if(!rrt.goalreached){
			FMS.NextGoal = goal;
		}
		
		
		Plan ResolutionPlan1 = rrt.GetPlan();
		Plan ResolutionPlan2 = ComputeGoAbovePlan(start,goal,altFence,resolutionSpeed);

		double length1 = ResolutionPlan1.pathDistance();
		double length2 = ResolutionPlan2.pathDistance();

		if( (altFence >= maxAlt) ){
			length2 = Double.MAX_VALUE;
		}

		FlightData.ResolutionPlan.clear();
		if(length1 < length2){
			FlightData.ResolutionPlan = ResolutionPlan1;
		}else{
			FlightData.ResolutionPlan = ResolutionPlan2;
		}

		FMS.planType        = plan_type_t.TRAJECTORY;
		FMS.resumeMission   = false;
		return;

	}
	
	public Plan ComputeGoAbovePlan(Position start,Position goal,double altFence,double rSpeed){
		// Compute go above plan
		Plan ResolutionPlan2 = new Plan();
		double ETA = 0;
		double distH,distV;

		NavPoint nvpt1 = new NavPoint(start,ETA);
		ResolutionPlan2.add(nvpt1);

		// Second waypoint directly above WP1
		Position wp2 = start.mkAlt(altFence+1);
		distV = wp2.distanceV(start);
		ETA = ETA + distV/rSpeed;
		NavPoint nvpt2 = new NavPoint(wp2,ETA);
		ResolutionPlan2.add(nvpt2);

		// Third waypoint directly above exit point
		Position wp3 = goal.mkAlt(altFence+1);
		distH = wp3.distanceH(wp2);
		ETA = ETA + distH/rSpeed;
		NavPoint nvpt3 = new NavPoint(wp3,ETA);
		ResolutionPlan2.add(nvpt3);

		// Final waypoint
		distV = goal.distanceH(wp3);
		ETA = ETA + distV/rSpeed;
		NavPoint nvpt4 = new NavPoint(goal,ETA);
		ResolutionPlan2.add(nvpt4);

		return ResolutionPlan2;
	}

	public void ResolveFlightPlanDeviationConflict(){
		xtrkDevGain       = FlightData.pData.getValue("XTRK_GAIN");
		
		allowedDev      = FlightData.pData.getValue("XTRK_DEV");
		resolutionSpeed          = (float) FlightData.pData.getValue("RES_SPEED");

		if(xtrkDevGain < 0){
			xtrkDevGain = - xtrkDevGain;
		}

		Plan currentFP = FlightData.MissionPlan;
		double elapsedTime = FMS.getApproxElapsedPlanTime(currentFP,FlightData.nextMissionWP);
		Position prevWP = currentFP.point(FlightData.nextMissionWP - 1).position();
		Position nextWP = currentFP.point(FlightData.nextMissionWP).position();


		if(Math.abs(FlightData.crossTrackDeviation) <= 2*allowedDev){
			double Vs = xtrkDevGain*FlightData.crossTrackDeviation;
			double V  = resolutionSpeed;

			double sgn = 0;

			if(Vs >= 0){
				sgn = 1;
			}
			else{
				sgn = -1;
			}

			if(Math.pow(Math.abs(Vs),2) >= Math.pow(V,2)){
				Vs = V*sgn;
			}

			double Vf  = Math.sqrt( Math.pow(V,2) - Math.pow(Vs,2) );
			double Trk = prevWP.track(nextWP);

			FlightData.maneuverVn = Vf*Math.cos(Trk) - Vs*Math.sin(Trk);
			FlightData.maneuverVe = Vf*Math.sin(Trk) + Vs*Math.cos(Trk);
			FlightData.maneuverVu = 0;
			FlightData.maneuverHeading = Math.toDegrees(Math.atan2(FlightData.maneuverVe,FlightData.maneuverVn));

			if(FlightData.maneuverHeading < 0){
				FlightData.maneuverHeading = 360 + FlightData.maneuverHeading;
			}

			//System.out.println("Ref heading:"+RefHeading2);
			FMS.planType = plan_type_t.MANEUVER;		
		}
		else{
	
			Position CurrentPos   = FlightData.acState.positionLast();
			Position cp = GetPointOnPlan(FlightData.crossTrackOffset,FlightData.MissionPlan,FlightData.nextMissionWP);
			
			FlightData.maneuverHeading = Math.toDegrees(CurrentPos.track(cp));

			if(FlightData.maneuverHeading < 0){
				FlightData.maneuverHeading = 360 + FlightData.maneuverHeading;
			}

			//System.out.format("dn = %f, de = %f\n",dn,de);
			//System.out.format("Offset:%f\n",crossTrackOffset);
			//System.out.format("Heading to next WP:%f\n",headingNextWP);
			//System.out.println("Closest point:"+cp.toString());
			//System.out.println("Ref heading in standoff resolution plan:"+RefHeading2);
			FlightData.ResolutionPlan.clear();
			FlightData.ResolutionPlan.add(new NavPoint(CurrentPos,0));
			double distance = CurrentPos.distanceH(cp);
			double ETA      = distance/resolutionSpeed;
			FlightData.ResolutionPlan.add(new NavPoint(cp,ETA));
			
			FMS.GoalReached = true;
			FMS.planType = plan_type_t.TRAJECTORY;
			FMS.resumeMission = false;
		}
		
	}
	
	public Position GetPointOnPlan(double offset,Plan fp,int next){
		
		Position nextWP       = fp.point(next).position();
		Position prevWP       = fp.point(next-1).position();
		double headingNextWP  = prevWP.track(nextWP);;
		double dn             = offset*Math.cos(headingNextWP);
		double de             = offset*Math.sin(headingNextWP);
		Position cp           = prevWP.linearEst(dn, de);
		
		if(cp.alt() <= 0){
			cp = cp.mkAlt(nextWP.alt());
		}
		
		return cp;
		
	}
	
	public void ResolveTrafficConflictDAA(){
		
		// Track based resolutions
		//TODO: change 0.1 based on pref direction
		boolean prefDirection = FMS.Detector.KMB.preferredTrackDirection(); 
		double prefHeading = FMS.Detector.KMB.trackResolution(prefDirection)+0.1;
		Interval conflictInterval = FMS.Detector.KMB.timeIntervalOfViolation(1);
		double conflictEndTime = conflictInterval.up;
		double resolutionSpeed = FMS.FlightData.pData.getValue("RES_SPEED");
		
		Position currentPos = FlightData.acState.positionLast();
		Velocity currentVel = FlightData.acState.velocityLast();
		Position goal       = null;
		Position nextPos    = currentPos.linearDist(prefHeading, Units.from(Units.meter, 10));
		Velocity nextVel;
		boolean returnPathConflict = true;
		boolean currentPathConflict = true;
		double nextHeading;
		
		Daidalus DAA = new Daidalus();
		KinematicBandsParameters bParams = new KinematicBandsParameters();
		bParams.loadFromFile("params/DaidalusQuadConfig.txt");
		double distance = bParams.alertor.getLevel(1).getDetector().getParameters().getValue("D", "m");
		double height   = bParams.alertor.getLevel(1).getDetector().getParameters().getValue("H", "m");
		bParams.alertor.getLevel(1).setDetector(new CDCylinder(distance*3, "m", height, "m"));
		
		System.out.println("***");
		System.out.println(FMS.Detector.KMB.outputString());
		// Get a point in the provided heading roughly 1 m ahead of current position.
		double len = 4;
		double alertTime = 0;
		KinematicMultiBands KMB;
		while(returnPathConflict || currentPathConflict){
			len++;
			nextPos     = currentPos.linearDist(prefHeading, Units.from(Units.meter, len));
			//nextVel     = Velocity.makeTrkGsVs(Units.convert(Units.rad, Units.deg, prefHeading), Units.convert(Units.meter_per_second, Units.knot, resolutionSpeed), 0);
			double crossStats[] = FMS.Detector.ComputeCrossTrackDev(nextPos, FlightData.MissionPlan, FlightData.nextMissionWP);
			goal = GetPointOnPlan(crossStats[1], FlightData.MissionPlan,FlightData.nextMissionWP);
			
			nextHeading = nextPos.track(goal);
			nextVel     = Velocity.makeTrkGsVs(Units.convert(Units.rad, Units.deg, nextHeading), Units.convert(Units.meter_per_second, Units.knot, resolutionSpeed), 0);
			alertTime   = nextPos.distanceH(goal)/resolutionSpeed;
			
			bParams.alertor.getLevel(1).setAlertingTime(alertTime+5+5);
			bParams.alertor.getLevel(1).setEarlyAlertingTime(alertTime + 5+5);
			DAA.parameters = bParams;
			
			DAA.setOwnshipState("ProjectedOwnship", nextPos, nextVel, 0);
			Position trafficPos = null;
			Velocity trafficVel = null;
			for(int i=0;i<FMS.FlightData.traffic.size();++i){
				trafficVel = FMS.FlightData.traffic.get(i).vel;
				trafficPos = FMS.FlightData.traffic.get(i).pos.linear(trafficVel, len/resolutionSpeed);
				DAA.addTrafficState("ResTraffic"+i, trafficPos, trafficVel);
			}
			
			
			KMB = DAA.getKinematicMultiBands();
			returnPathConflict  = KMB.regionOfTrack(nextHeading).isConflictBand();
			currentPathConflict = KMB.regionOfTrack(prefHeading).isConflictBand();
			
			if(!returnPathConflict){
				returnPathConflict = (KMB.regionOfTrack(nextHeading) == BandsRegion.RECOVERY);
			}
			
			int count = 0;
			for(int i=0;i<KMB.trackLength();++i){
				Interval iv = KMB.track(i, "deg"); // i-th band region
	            double lower_trk = iv.low; // [deg]
	            double upper_trk = iv.up; // [deg]
	            BandsRegion regionType = KMB.trackRegion(i);
	            if (regionType.toString() != "NONE") {
	            	if(FMS.Detector.CheckTurnConflict(lower_trk, upper_trk, Units.convert(Units.rad, Units.deg, nextHeading),  Units.convert(Units.rad, Units.deg, prefHeading))){
	            		count++;
	            	}
	            }
			}
			
			if(count>0){
				returnPathConflict = true;
			}
			
			//System.out.println(KMB.outputString());
			//System.out.println(returnPathConflict);
			System.out.println("len:"+len);
			if(len <= 1){
				System.out.println(DAA.toString());
			}
			System.out.println(KMB.outputString());
			System.out.println(KMB.core_.ownship.get_eprj().project(currentPos).toString());
			System.out.println(KMB.core_.ownship.get_eprj().project(nextPos).toString());
			System.out.println(nextVel.toString());
			System.out.println(KMB.core_.ownship.get_eprj().project(goal).toString());
			System.out.println(KMB.core_.ownship.get_eprj().project(trafficPos).toString());
		}
		
		
		Position start = currentPos.linearDist(prefHeading, 0.25);
		double ETA2    = start.distanceH(nextPos)/resolutionSpeed;
		double ETA3    = ETA2 + nextPos.distanceH(goal)/resolutionSpeed;
		NavPoint wp1   = new NavPoint(start,0);
		NavPoint wp2   = new NavPoint(nextPos,ETA2);
		NavPoint wp3   = new NavPoint(goal,ETA3);
		
		FMS.FlightData.ResolutionPlan.clear();
		FMS.FlightData.ResolutionPlan.add(wp1);
		FMS.FlightData.ResolutionPlan.add(wp2);
		FMS.FlightData.ResolutionPlan.add(wp3);
		
		FMS.GoalReached = true;
		FMS.planType = plan_type_t.TRAJECTORY;
		FMS.resumeMission = true;
		
		FMS.Detector.Clear();
		FMS.Detector.numConflicts = 0;
		
	}

	public void ResolveTrafficConflictRRT(){
		// Reroute flight plan
		FMS.SetMode(ARDUPILOT_MODES.GUIDED); // Set mode to guided for quadrotor to hover before replanning

		ArrayList<Position> TrafficPos = new ArrayList<Position>();
		ArrayList<Velocity> TrafficVel = new ArrayList<Velocity>();

		Position currentPos = FlightData.acState.positionLast();
		Velocity currentVel = FlightData.acState.velocityLast();

		
		for(int i=0;i<FlightData.traffic.size();++i){
			Position tPos = FlightData.traffic.get(i).pos;
			Velocity tVel = FlightData.traffic.get(i).vel;
			TrafficPos.add(tPos);
			TrafficVel.add(tVel);
		}

		Plan currentFP;
		Position prevWP = null;
		Position nextWP = null;
		Position start = currentPos;
		double elapsedTime;

		if(FMS.planType == plan_type_t.MISSION){
			currentFP = FlightData.MissionPlan;
			elapsedTime = FMS.getApproxElapsedPlanTime(currentFP,FlightData.nextMissionWP);
			prevWP = currentFP.point(FlightData.nextMissionWP - 1).position();
			nextWP = currentFP.point(FlightData.nextMissionWP).position();
		}
		else if(FMS.planType == plan_type_t.TRAJECTORY){
			currentFP = FlightData.ResolutionPlan;
			elapsedTime = FMS.getApproxElapsedPlanTime(currentFP,FlightData.nextResolutionWP);
			prevWP = currentFP.point(FlightData.nextResolutionWP - 1).position();
			nextWP = currentFP.point(FlightData.nextResolutionWP).position();
		}

		Position goal = nextWP;

		int Nsteps = 500;
		int Tstep  = 5;
		double dT  = 1;
		RRT rrt = new RRT(FlightData.fenceList,start,currentVel,TrafficPos,TrafficVel,Tstep,dT);
		rrt.SetGoal(goal);

		for(int i=0;i<Nsteps;i++){
			rrt.RRTStep(i);
			if(rrt.CheckGoal()){
				//printf("Goal found\n");
				break;
			}
		}

		FMS.GoalReached = rrt.goalreached;
		if(!rrt.goalreached){
			FMS.NextGoal = goal;
		}

		FlightData.ResolutionPlan.clear();
		FlightData.ResolutionPlan = rrt.GetPlan();
		FMS.planType        = plan_type_t.TRAJECTORY;
		FMS.resumeMission   = false;
		return;
	}

}
