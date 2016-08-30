/**
 * Flight Safety Assessment and Management (FSAM)
 * Contact: Swee Balachandran (swee.balachandran@nianet.org)
 * 
 * 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.ICAROUS;

import java.util.*;
import java.lang.*;
import java.io.*;
import com.MAVLink.common.*;
import com.MAVLink.enums.*;

import gov.nasa.larcfm.Util.*;
import gov.nasa.larcfm.ACCoRD.*;


enum FSAM_OUTPUT{
    CONFLICT,NOOP, TERMINATE
}

enum RESOLVE_STATE{

    COMPUTE, EXECUTE_MANEUVER, EXECUTE_PLAN, MONITOR, JOIN, RESUME, NOOP
}

enum EXECUTE_STATE{
    START, SEND_COMMAND, AWAIT_COMPLETION, COMPLETE, PAUSE
}

public class FSAM{

    private Aircraft UAS;
    private AircraftData FlightData;
    private MAVLinkMessages Inbox;
    private Mission mission;
    private Interface apIntf;
    private double distance2WP;
    private double heading2WP;

    private long timeEvent1;
    private long timeElapsed;
    private long timeCurrent;
    private double logtime;
    
    private FSAM_OUTPUT output;

    private List<Conflict> conflictList;
    
    private Plan ResolutionPlan;
    
    private int currentResolutionWP;

    public RESOLVE_STATE resolveState;
    public EXECUTE_STATE executeState;

    private boolean FenceKeepInConflict;
    private boolean FenceKeepOutConflict;
    private boolean TrafficConflict;    
    private boolean StandoffConflict;
    private boolean GotoNextWP;
    private boolean joined;
    
    
    private int currentConflicts;
    private boolean NominalPlan;

    
    private double Vn;
    private double Ve;
    private double Vu;
    private double RefHeading1;
    private double RefHeading2;
    
    public double crossTrackDeviation;

    private Daidalus daa;
    private int daaTick;
    private KinematicMultiBands KMB;

    private long pausetime_start;
    
    public FSAM(Aircraft ac,Mission ms){
	UAS                      = ac;
	FlightData               = ac.FlightData;
	Inbox                    = FlightData.Inbox;
	mission                  = ms;
	timeEvent1               = 0;
	timeElapsed              = 0;
	apIntf                   = ac.apIntf;
	conflictList             = new ArrayList<Conflict>();
	ResolutionPlan           = new Plan();
	resolveState             = RESOLVE_STATE.NOOP;
	executeState             = EXECUTE_STATE.COMPLETE;
	FenceKeepInConflict      = false;
	FenceKeepOutConflict     = false;
	TrafficConflict          = false;
	StandoffConflict         = false;	
	NominalPlan              = true;
	currentResolutionWP      = 0;
	currentConflicts         = 0;
	joined                   = true;
	RefHeading1              = Double.NaN;
	
	
	// Create an object of type Daidalus for a well-clear volume based on TAUMOD
	daa = new Daidalus();

	ParameterData p = new ParameterData();
		
	p.setInternal("min_gs", Units.from("m/s",0), "kts");
	p.setInternal("max_gs", Units.from("m/s",4), "kts");
	p.setInternal("min_vs", Units.from("fpm",0), "fpm");
	p.setInternal("max_vs", Units.from("fpm",100), "fpm");
	p.setInternal("min_alt", Units.from("ft",0), "ft");
	p.setInternal("max_alt", Units.from("ft",500), "ft");
	
	// Kinematic bands
	
	p.setInternal("horizontal_accel", Units.from("m/s^2",1), "m/s^2");
	p.setInternal("vertical_accel", Units.from("m/s^2",1), "m/s^2");
	p.setInternal("turn_rate", Units.from("deg/s",60), "deg/s");
	p.setInternal("bank_angle", Units.from("deg",60), "deg");
	p.setInternal("vertical_rate", Units.from("fpm",200), "fpm");
	
	
	daa.parameters.setParameters(p);			
	daa.parameters.setLookaheadTime(50);
	daa.parameters.setCollisionAvoidanceBands(true);
	daa.parameters.alertor = AlertQuad();

	daaTick = 0;
	KMB = null;
    }

    static public AlertLevels AlertQuad() {		
	AlertLevels alertor = new AlertLevels();
	alertor.setConflictAlertLevel(1);		
	alertor.add(new AlertThresholds(new CDCylinder(10,"m",1,"m"),30,40,BandsRegion.NEAR));
	return alertor;
    }

    // Returns time corresponding to the current position in the resolution flight plan
    // (assuming constant velocity throughtout flight plan)
    public double GetResolutionTime(){
	Plan FP = ResolutionPlan;

	
	double legDistance, legTime, lastWPDistance, currentTime;
	Position pos = FlightData.acState.positionLast();

	if(currentResolutionWP == 0){
	    return 0;
	}

	currentTime = 0;
	if(currentResolutionWP < ResolutionPlan.size()){	    	    
	    legTime        = FP.getTime(currentResolutionWP) - FP.getTime(currentResolutionWP-1);
	    legDistance    = FP.pathDistance(currentResolutionWP-1);
	    lastWPDistance = FP.point(currentResolutionWP-1).position().distanceH(pos);
	    currentTime    = FP.getTime(currentResolutionWP-1) + legTime/legDistance * lastWPDistance;
	}				
	

	return currentTime;
    }

    // Function to monitor conflicts (Geofence, flight plan deviation, air traffic etc...)
    public FSAM_OUTPUT Monitor(){

	Plan FlightPlan       = FlightData.CurrentFlightPlan;
	AircraftState acState = FlightData.acState;

	
	timeCurrent    = UAS.timeCurrent;	
	timeElapsed    = timeCurrent - timeEvent1;
	
	if(timeEvent1 == 0){
	    timeEvent1 = UAS.timeStart;
	}

	//Get distance to next waypoint
	Position wp         = FlightPlan.point(FlightData.FP_nextWaypoint).position();
	Position currentPos = acState.positionLast();	
	double distance2WP  = currentPos.distanceH(wp);
	
	// Get time of current position in nominal plan
	UAS.FlightData.getPlanTime();
		
	// Check for geofence resolutions.
	CheckGeoFences();
	
	// Check for deviation from prescribed flight profile only in the NOOP state.	
	CheckStandoff();
		    
	// Check for conflicts from DAIDALUS against other traffic.
	if(FlightData.traffic.size() > 0){
	    CheckTraffic();
	}

	// Check mission progress.
	if(timeElapsed > 5E9){
	    timeEvent1 = timeCurrent;      
	    //System.out.format("Distance to next waypoint: %2.2f \n",distance);
	}

	// Check if next mission item (waypoint) has been reached
	if(CheckMissionItemReached()){
	    Plan CurrentFlightPlan = FlightData.CurrentFlightPlan;
	    UAS.error.addWarning("[" + UAS.timeLog + "] MSG: Reached waypoint");
	    FlightData.FP_nextWaypoint++;

	    if(FlightData.FP_nextWaypoint < FlightData.FP_numWaypoints){
		float speed = UAS.GetSpeed();					
		UAS.SetSpeed(speed);
		UAS.error.addWarning("[" + UAS.timeLog + "] CMD:SPEED CHANGE TO "+speed+" m/s");
	    }
	}
	
	// If conflicts are detected, initialize the state machine for the resolution function	
	if(conflictList.size() != currentConflicts){
	    currentConflicts = conflictList.size();
	    resolveState = RESOLVE_STATE.COMPUTE;
	    UAS.error.addWarning("[" + UAS.timeLog + "] MSG: Conflict(s) detected");
	    return FSAM_OUTPUT.CONFLICT;	    
	}		 	
	
	if(resolveState == RESOLVE_STATE.NOOP){
	    return FSAM_OUTPUT.NOOP;
	}else{	    
	    return FSAM_OUTPUT.CONFLICT;	    
	}


	
    }

    // Function to compute resolutions for conflicts
    public int Resolve(){

	int status;
	Plan CurrentFP  = FlightData.CurrentFlightPlan;
	boolean UsePlan = true;
	float resolutionSpeed   = (float) UAS.pData.getValue("RES_SPEED");
	
	switch(resolveState){

	case COMPUTE:
	    // Call the relevant resolution functions to resolve conflicts
	    // [TODO:] Add conflict resolution table to add prioritization/scheduling to handle multiple conflicts
	    if(TrafficConflict){
		UAS.error.addWarning("[" + UAS.timeLog + "] MSG: Computing resolution for traffic conflict");		
		UsePlan = false;
		// Resolution will be computed during the manuever
	    }
	    else if(FenceKeepInConflict){
		UAS.error.addWarning("[" + UAS.timeLog + "] MSG: Computing resolution for keep in conflict");
		UAS.SetSpeed(resolutionSpeed);
		ResolveKeepInConflict();		
	    }
	    else if(FenceKeepOutConflict){
	       UAS.error.addWarning("[" + UAS.timeLog + "] MSG: Computing resolution for keep out conflict");
	       UAS.SetSpeed(resolutionSpeed);
	       ResolveKeepOutConflict();		
	    }
	    else if(StandoffConflict){
		UAS.error.addWarning("[" + UAS.timeLog + "] MSG: Computing resolution for stand off conflict");	    
		ResolveStandoffConflict();
		UsePlan = false;
	    }

	    // Two kinds of actions to resolve conflicts : 1. Plan based resolution, 2. Maneuver based resolution
	    if(UsePlan){
		resolveState = RESOLVE_STATE.EXECUTE_PLAN;
		executeState = EXECUTE_STATE.START;
		NominalPlan  = false;
		joined       = false;
	    }
	    else{
		resolveState = RESOLVE_STATE.EXECUTE_MANEUVER;
		executeState = EXECUTE_STATE.START;
	    }
	    	    
	    break;

	case EXECUTE_MANEUVER:
	    // Execute maneuver based resolution when appropriate
	    
	    if( executeState != EXECUTE_STATE.COMPLETE ){

		if(TrafficConflict){
		    ResolveTrafficConflict();
		}
		else if(StandoffConflict){
		    ResolveStandoffConflict();
		}
		
		ExecuteManeuver();
	    }
	    else{
		UAS.apMode = Aircraft.AP_MODE.AUTO;
		UAS.SetMode(3);
		UAS.error.addWarning("[" + UAS.timeLog + "] MODE: AUTO");
		resolveState = RESOLVE_STATE.NOOP;
	    }
	    
	    break;

	case EXECUTE_PLAN:
	    // Execute plan based resolution when appropriate
	    if( executeState != EXECUTE_STATE.COMPLETE ){
		ExecuteResolutionPlan();
	    }
	    else{
		if(joined){
		    resolveState = RESOLVE_STATE.RESUME;
		}else{
		    resolveState = RESOLVE_STATE.JOIN;
		}
	    }
	    
	    break;

	case JOIN:	    

	    // Once resolution is complete, join the original mission
	    ResolutionPlan.clear();
	    Position pos = FlightData.acState.positionLast();
	    Position nextWP = FlightData.CurrentFlightPlan.point(FlightData.FP_nextWaypoint).position();
	    
	    ResolutionPlan.add(new NavPoint(pos,0));	    	    	    
	    double distance = nextWP.distanceH(pos);
	    double ETA      = distance/resolutionSpeed;
	    ResolutionPlan.add(new NavPoint(nextWP,ETA));

	    resolveState = RESOLVE_STATE.EXECUTE_PLAN;
	    executeState = EXECUTE_STATE.START;
	    UAS.error.addWarning("[" + UAS.timeLog + "] MSG: Joining mission");
	    joined = true;

	    Iterator Itr = conflictList.iterator();
	    while(Itr.hasNext()){
		Conflict cf = (Conflict) Itr.next();
		Itr.remove();
	    }
	    currentConflicts = conflictList.size();
	    
	    break;
	    

	case RESUME:
	    // Continue original mission	  
	    msg_mission_set_current msgMission = new msg_mission_set_current();
	    msgMission.target_system     = 0;
	    msgMission.target_component  = 0;
	    msgMission.seq               = FlightData.FP_nextWaypoint;	    		
	    UAS.error.addWarning("[" + UAS.timeLog + "] CMD: Set next mission item: "+msgMission.seq);
	    UAS.apIntf.Write(msgMission);

	    UAS.apMode = Aircraft.AP_MODE.AUTO;
	    UAS.SetMode(3);
	    UAS.error.addWarning("[" + UAS.timeLog + "] MODE: AUTO");
	    resolveState = RESOLVE_STATE.NOOP;
	    NominalPlan  = true;
	    
	    break;

	case NOOP:

	    break;
	    
	}
	
	return 0;
    }

    
    public boolean CheckMissionItemReached(){

	boolean reached = false;

	msg_mission_item_reached msgMissionItemReached = Inbox.GetMissionItemReached();
	if(msgMissionItemReached != null){
	    reached = true;
	}


	return reached;	
    }        

    // Check for geofence violations
    public void CheckGeoFences(){

	 FenceKeepInConflict  = false;
	 FenceKeepOutConflict = false;

	 Plan CurrentPlan;
	 double planTime;
	 
	 if(NominalPlan){
	     CurrentPlan =  UAS.FlightData.CurrentFlightPlan;
	     planTime    =  UAS.FlightData.planTime;
	 }
	 else{
	     CurrentPlan = ResolutionPlan;
	     planTime    = GetResolutionTime();
	 }
	 
	 for(int i=0;i< FlightData.fenceList.size();i++){
	     
	    GeoFence GF = (GeoFence) FlightData.fenceList.get(i);

	    GF.CheckViolation(FlightData.acState,planTime,CurrentPlan);

	    Conflict cf;	   
	    
	    
	    if(GF.conflict){
		
		if(GF.Type == 0){
		    cf = new Conflict(PRIORITY_LEVEL.MEDIUM,CONFLICT_TYPE.KEEP_IN,GF,FlightData.acState.positionLast());
		    FenceKeepInConflict = true;
		}
		else{
		    cf = new Conflict(PRIORITY_LEVEL.MEDIUM,CONFLICT_TYPE.KEEP_OUT,GF,FlightData.acState.positionLast());
		    FenceKeepOutConflict = true;
		}
		Conflict.AddConflictToList(conflictList,cf);
		
	    }
	    else{
		//if(GF.Type == 0){
		//    cf = new Conflict(PRIORITY_LEVEL.MEDIUM,CONFLICT_TYPE.KEEP_IN,GF,FlightData.acState.positionLast());
		//    Conflict.RemoveConflict(conflictList,cf);
		//}		
		
	    }
	}
	
    }

    // Check standoff distance violation
    public void CheckStandoff(){

	double standoff          = UAS.pData.getValue("STANDOFF");
		
	double heading = FlightData.yaw;

	if(heading < 0){
	    heading = 360 + heading;
	}
	    
	double heading_fp_pos;

	Position PrevWP = FlightData.CurrentFlightPlan.point(FlightData.FP_nextWaypoint - 1).position();
	Position NextWP = FlightData.CurrentFlightPlan.point(FlightData.FP_nextWaypoint).position();

	Position CurrentPos = FlightData.acState.positionLast();

	double psi1 = PrevWP.track(NextWP) * 180/Math.PI;
	double psi2 = PrevWP.track(CurrentPos) * 180/Math.PI;

	double sgn = 0;
	
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

	double bearing = Math.abs(psi1 - psi2);

	double dist = PrevWP.distanceH(CurrentPos);

	crossTrackDeviation = sgn*dist*Math.sin(Math.toRadians(bearing));

	if(Math.abs(crossTrackDeviation) > standoff){
	    StandoffConflict = true;	    
	    Conflict cf = new Conflict(PRIORITY_LEVEL.MEDIUM,CONFLICT_TYPE.FLIGHTPLAN);
	    Conflict.AddConflictToList(conflictList,cf);
	}else if(Math.abs(crossTrackDeviation) < (standoff)/3){
	    StandoffConflict = false;
	}
    }

    public void CheckTraffic(){
	daaTick = daaTick+1;
	Position so = FlightData.acState.positionLast();
	Velocity vo = FlightData.acState.velocityLast();	
	
	daa.reset();
	daa.setOwnshipState("Ownship",so,vo,0.0);

	double dist = Double.MAX_VALUE;
	for(int i=0;i<FlightData.traffic.size();i++){
	    synchronized(FlightData.traffic){
		Position si = FlightData.traffic.get(i).pos.copy();
		Velocity vi = FlightData.traffic.get(i).vel.mkAddTrk(0);
		
		daa.addTrafficState("traffic"+i,si,vi);

		double trafficdist = so.distanceH(si);

		if(trafficdist < dist){
		    dist = trafficdist;
		}
	    }
	    
	}

	if(daaTick > 100){	    
	    if(dist > 15){
		if(TrafficConflict){
		    TrafficConflict = false;
		    pausetime_start = UAS.timeCurrent;
		    executeState    = EXECUTE_STATE.PAUSE;
		    RefHeading1     = Double.NaN;
		}	    	    
	    }
	}
	
	for (int ac=1; ac < daa.numberOfAircraft(); ac++) {
	    double tlos = daa.timeToViolation(ac);
	    if (tlos >= 0) {
		TrafficConflict = true;
		System.out.printf(
				  "Predicted violation with traffic aircraft %s in %.1f [s]\n",
				  daa.getAircraftState(ac).getId(),tlos);

		Conflict cf = new Conflict(PRIORITY_LEVEL.HIGH,CONFLICT_TYPE.TRAFFIC);
		Conflict.AddConflictToList(conflictList,cf);
		daaTick = 0;
		//System.out.println(daa.toString());

		KMB = daa.getKinematicMultiBands();
		System.out.println(KMB.outputString());
	    }
	}
	
		
    }
    
    
    


    // Compute resolution for keep in conflict
    public void ResolveKeepInConflict(){

	Plan CurrentFP = FlightData.CurrentFlightPlan;
	GeoFence GF = null;

	for(int i=0;i<conflictList.size();i++){
	    Conflict CF = conflictList.get(i);
	    if(CF.conflictType == CONFLICT_TYPE.KEEP_IN){
		GF = CF.fence;
		break;
	    }
	}

	NavPoint wp = null;
	if(GF.violation){
	    wp = new NavPoint(GF.RecoveryPoint,0);
	}
	else{
	    wp = new NavPoint(GF.SafetyPoint,0);
	}
	
	
	ResolutionPlan.clear();
	currentResolutionWP = 0;
	
	ResolutionPlan.add(wp);
	
	NavPoint nextWP = CurrentFP.point(FlightData.FP_nextWaypoint);
	if(!GF.CheckWaypointFeasibility(wp.position(),nextWP.position())){
	    GotoNextWP = true;
	    FlightData.FP_nextWaypoint++;	    
	    
	}else{
	    GotoNextWP = false;
	}
	

    }

    // Compute resolution for keep out conflicts
    public void ResolveKeepOutConflict(){

	Plan CurrentFP;
	double currentTime;
	float resolutionSpeed   = (float) UAS.pData.getValue("RES_SPEED");
	double gridsize          = UAS.pData.getValue("GRIDSIZE");
	double buffer            = UAS.pData.getValue("BUFFER");
	double lookahead         = UAS.pData.getValue("LOOKAHEAD");
	double proximityfactor   = UAS.pData.getValue("PROXFACTOR");
	
	if(NominalPlan){
	    CurrentFP = FlightData.CurrentFlightPlan;
	    currentTime = UAS.FlightData.planTime;
	}
	else{
	    CurrentFP   = ResolutionPlan;
	    currentTime = GetResolutionTime();
	}

	double minTime = Double.MAX_VALUE;
	double maxTime = 0.0;

	ArrayList<PolyPath> LPP = new ArrayList<PolyPath>();
	ArrayList<PolyPath> CPP = new ArrayList<PolyPath>();
	WeatherUtil WU = new WeatherUtil();

	Position RecoveryPoint = null;
	boolean violation = false;
	
	// Get conflict start and end time
	for(int i=0;i<conflictList.size();i++){

	    if(conflictList.get(i).conflictType != CONFLICT_TYPE.KEEP_OUT){
		continue;
	    }
	    
	    GeoFence GF = (GeoFence) conflictList.get(i).fence;
	   	    
	    if(GF.entryTime <= minTime){
		minTime = GF.entryTime;
	    }
	    
	    if(GF.exitTime >= maxTime){
		maxTime = GF.exitTime;
	    }
	    
	    LPP.add(GF.geoPolyPath);

	    if(GF.violation){
		RecoveryPoint = GF.RecoveryPoint;
		violation = true;
	    }
	    
	}

	CPP.add(FlightData.fenceList.get(0).geoPolyPath);
	
	minTime = minTime - lookahead;
	maxTime = maxTime + lookahead;
	
	if(minTime < currentTime){
	    minTime = currentTime+0.1;
	}

	if(maxTime > CurrentFP.getLastTime()){
	    maxTime = CurrentFP.getLastTime() - 0.1;
	}

	 
	if(NominalPlan){
	    FlightData.FP_nextWaypoint = CurrentFP.getSegment(maxTime)+1;
	}
	
	// Get flight plan between start time and end time (with a 3 second buffer on both sides)
	Plan ConflictFP = PlanUtil.cutDown(CurrentFP,minTime,maxTime);
	
	// Reroute flight plan
	UAS.SetMode(4); // Set mode to guided for quadrotor to hover before replanning

	// Create a bounding box based on containment geofence
	BoundingRectangle BR = new BoundingRectangle();

	SimplePoly CF = FlightData.fenceList.get(0).geoPolyLLA;

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

	for(int i=1;i<FlightData.fenceList.size();i++){
	    GeoFence GF = FlightData.fenceList.get(i);
	    double hthreshold = UAS.pData.getValue("HTHRESHOLD");
	    double vthreshold = UAS.pData.getValue("VTHRESHOLD");
	    SimplePoly expfence = GF.pu.bufferedConvexHull(GF.geoPolyLLA,hthreshold,vthreshold);
	    dg.setWeightsInside(expfence,100.0);
	}

	// Perform A* seartch
	List<Pair <Integer,Integer>> GridPath = dg.optimalPath();

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
	ResolutionPlan.clear();
	currentResolutionWP = 0;
	double ETA   = 0.0;
	ResolutionPlan.add(new NavPoint(PlanPosition.get(0),ETA));
	for(int i=1;i<PlanPosition.size();i++){
	    Position pos = PlanPosition.get(i);
	    double distance = pos.distanceH(PlanPosition.get(i-1));
	    ETA      = ETA + distance/resolutionSpeed;

	    ResolutionPlan.add(new NavPoint(pos,ETA));
	}

	
	
    }

    // Compute resolution for stand off distance violation
    public void ResolveStandoffConflict(){


	double XtrkDevGain       = UAS.pData.getValue("XTRK_GAIN") ;

	if(XtrkDevGain < 0){
	    XtrkDevGain = - XtrkDevGain;
	}
	
	if(StandoffConflict){
	    double Vs = XtrkDevGain*crossTrackDeviation;
	    double V  = UAS.GetSpeed();

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

	    double Vf       = Math.sqrt( Math.pow(V,2) - Math.pow(Vs,2) );

	    Position PrevWP = FlightData.CurrentFlightPlan.point(FlightData.FP_nextWaypoint - 1).position();
	    Position NextWP = FlightData.CurrentFlightPlan.point(FlightData.FP_nextWaypoint).position();

	    double Trk = PrevWP.track(NextWP);

	    Vn = Vf*Math.cos(Trk) - Vs*Math.sin(Trk);
	    Ve = Vf*Math.sin(Trk) + Vs*Math.cos(Trk);
	    Vu = 0;
	    RefHeading2 = Math.toDegrees(Math.atan2(Ve,Vn));

	    if(RefHeading2 < 0){
		RefHeading2 = 360 + RefHeading2;
	    }
	    
	    //System.out.format("Vs = %f, Vf = %f,V=%f,G=%f,Xdev=%f\n",Vs,Vf,V,XtrkDevGain,crossTrackDeviation);
	    //System.out.format("Trk = %f,Vn=%f,Ve=%f\n",Trk,Vn,Ve);
	    //System.out.println("Ref heading:"+RefHeading2);
	}
	else{
	    Vn = 0;
	    Ve = 0;
	    Vu = 0;
	}
    }

    public void ResolveTrafficConflict(){		
	
	double heading_right = KMB.trackResolution(true);
	double heading_left  = KMB.trackResolution(false);
	double res_heading;
		
	//System.out.println("resolution heading L:"+heading_left*180/3.142);
	//System.out.println("resolution heading R:"+heading_right*180/3.142);

	heading_left  = heading_left*180/Math.PI;
	heading_right = heading_right*180/Math.PI;
	res_heading   = Double.NaN;
	double d1,d2,h1,h2, diff = Double.MAX_VALUE;
	if(KMB.trackLength() > 1){	    

	    Position CurrentPos = FlightData.acState.positionLast();
	    Position NextWP     = FlightData.CurrentFlightPlan.point(FlightData.FP_nextWaypoint).position();
	    double planTrack    = Math.toDegrees(CurrentPos.track(NextWP));  	    
	    
	    d1 = Math.abs(planTrack - heading_left);
	    d2 = Math.abs(planTrack - heading_right);

	    if(d1 <= d2){
		res_heading = heading_left;
	    }
	    else{
		res_heading = heading_right;
	    }

	    for(int i=0;i<KMB.trackLength();i++){
		Interval iv = KMB.track(i,"deg"); //i-th band region
		double lower_trk = iv.low; //[deg]
		double upper_trk = iv.up; //[deg]
		BandsRegion regionType = KMB.trackRegion(i);		
		if (regionType.toString() == "<NONE>" ){		    
		    if (planTrack >= lower_trk && planTrack <= upper_trk){
			res_heading = planTrack;
		    }
		}
	    }	    
	}
	else{
	    
	    if(Double.isNaN(RefHeading1)){
	       Position PrevWP     = FlightData.CurrentFlightPlan.point(FlightData.FP_nextWaypoint - 1).position();
	       Position NextWP     = FlightData.CurrentFlightPlan.point(FlightData.FP_nextWaypoint).position();
	       res_heading = Math.toDegrees(PrevWP.track(NextWP));
	    }
	}		
	
	if(!Double.isNaN(res_heading)){
	    double V  = UAS.GetSpeed();

	    double Vres = KMB.groundSpeedResolution(true);
	    if(!Double.isNaN(Vres) && !Double.isInfinite(Vres)){
		V = Math.ceil(Vres);		
	    }
	    else{
		//System.out.println("Resolution speed:"+V);
	    }

	    //System.out.println("Resolution speed:"+V);
	    Vn = V*Math.cos(Math.toRadians(res_heading));
	    Ve = V*Math.sin(Math.toRadians(res_heading));
	    Vu = 0;
	    RefHeading1 = res_heading;
	    //System.out.println("resolution heading:"+res_heading);
	    //System.out.format("Vn = %f, Ve = %f, Vu = %f\n",Vn,Ve,Vu);
	}
	else{	    
	    res_heading = RefHeading1;
	    double V  = UAS.GetSpeed();
	    Vn = V*Math.cos(Math.toRadians(res_heading));
	    Ve = V*Math.sin(Math.toRadians(res_heading));
	    Vu = 0;
	    //System.out.format("Vn = %f, Ve = %f, Vu = %f\n",Vn,Ve,Vu);
	}
    }
    

    public void ExecuteManeuver(){


	switch(executeState){
	    
	case START:
	    UAS.error.addWarning("[" + UAS.timeLog + "] MSG: Starting resolution");	    
	    UAS.apMode = Aircraft.AP_MODE.GUIDED;
	    UAS.SetMode(4);
	    try{
		Thread.sleep(500);
	    }
	    catch(InterruptedException e){
		System.out.println(e);
	    }
	    
	    currentResolutionWP = 0;
	    executeState = EXECUTE_STATE.SEND_COMMAND;
	    break;

	case SEND_COMMAND:

	    //System.out.format("Vn,Ve,Vu = %f,%f,%f\n",Vn,Ve,Vu);
	    UAS.SetVelocity(Vn,Ve,Vu);

	    if(TrafficConflict){
		//UAS.SetYaw(RefHeading1);
	    }else{
		UAS.SetYaw(RefHeading2);
	    }
	    

	    if(!StandoffConflict && !TrafficConflict){
		executeState = EXECUTE_STATE.COMPLETE;

		Iterator Itr = conflictList.iterator();
		while(Itr.hasNext()){
		    Conflict cf = (Conflict) Itr.next();
		    Itr.remove();
		}
		currentConflicts = conflictList.size();
		System.out.println("Finished maneuver");
	    }
	    
	    break;

	case PAUSE:

	    UAS.SetVelocity(0.0,0.0,0.0);

	    System.out.println("PAUSE:"+(float)((UAS.timeCurrent - pausetime_start))/1E9);
	    if((float) ((UAS.timeCurrent - pausetime_start))/1E9 > 3){
		executeState =EXECUTE_STATE.SEND_COMMAND;
	    }

	    break;
	}

    
    }
    
    public void ExecuteResolutionPlan(){	
	
	switch(executeState){

	case START:
	    UAS.error.addWarning("[" + UAS.timeLog + "] MSG: Starting resolution");	    
	    UAS.apMode = Aircraft.AP_MODE.GUIDED;
	    UAS.SetMode(4);
	    try{
		Thread.sleep(500);
	    }
	    catch(InterruptedException e){
		System.out.println(e);
	    }
	    
	    currentResolutionWP = 0;
	    executeState = EXECUTE_STATE.SEND_COMMAND;
	    break;
	    
	case SEND_COMMAND:

	    NavPoint wp = ResolutionPlan.point(currentResolutionWP);
	    UAS.SetGPSPos(wp.lla().latitude(),wp.lla().longitude(),wp.alt());
	    executeState = EXECUTE_STATE.AWAIT_COMPLETION;
	    break;
	    
	case AWAIT_COMPLETION:
	    
	    Position pos   = ResolutionPlan.point(currentResolutionWP).position();
	    double dist    = pos.distanceH(UAS.FlightData.acState.positionLast());

	    if(dist < 1){

		currentResolutionWP = currentResolutionWP + 1;
		if(currentResolutionWP < ResolutionPlan.size()){
		    executeState = EXECUTE_STATE.SEND_COMMAND;
		}
		else{		    
		    UAS.error.addWarning("[" + UAS.timeLog + "] MSG: Resolution complete");
		    executeState = EXECUTE_STATE.COMPLETE;
		    float speed = UAS.GetSpeed();					
		    UAS.SetSpeed(speed);
		    UAS.error.addWarning("[" + UAS.timeLog + "] CMD:SPEED CHANGE TO "+speed+" m/s");
		    
		}
	    }
	    
	    break;
	    
	    
	}//end switch case


    }//end function

    
    
}
