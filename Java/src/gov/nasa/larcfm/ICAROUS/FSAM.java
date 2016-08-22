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

import gov.nasa.larcfm.Util.Plan;
import gov.nasa.larcfm.Util.AircraftState;
import gov.nasa.larcfm.Util.Position;
import gov.nasa.larcfm.Util.NavPoint;
import gov.nasa.larcfm.Util.ParameterData;
import gov.nasa.larcfm.Util.PolyPath;
import gov.nasa.larcfm.Util.PlanUtil;
import gov.nasa.larcfm.Util.Pair;
import gov.nasa.larcfm.Util.WeatherUtil;
import gov.nasa.larcfm.Util.DensityGrid;
import gov.nasa.larcfm.Util.BoundingRectangle;
import gov.nasa.larcfm.Util.SimplePoly;
import gov.nasa.larcfm.IO.SeparatedInput;

enum FSAM_OUTPUT{
    CONFLICT,NOOP, TERMINATE
}

enum RESOLVE_STATE{

    COMPUTE, EXECUTE_MANEUVER, EXECUTE_PLAN, MONITOR, JOIN, RESUME, NOOP
}

enum EXECUTE_STATE{
    START, SEND_COMMAND, AWAIT_COMPLETION, COMPLETE
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
    
    private float resolutionSpeed;
    private int currentConflicts;
    private boolean NominalPlan;

    private double gridsize;
    private double buffer;
    private double lookahead;
    private double proximityfactor;
    private double standoff;
    private double XtrkDevGain;
    private double Vn;
    private double Ve;
    private double Vu;

    public double crossTrackDeviation;
    
    public FSAM(Aircraft ac,Mission ms){
	UAS = ac;
	FlightData = ac.FlightData;
	Inbox   = FlightData.Inbox;
	mission = ms;
	timeEvent1 = 0;
	timeElapsed = 0;
	apIntf = ac.apIntf;
	conflictList   = new ArrayList<Conflict>();
	ResolutionPlan = new Plan();
	resolveState = RESOLVE_STATE.NOOP;
	executeState = EXECUTE_STATE.COMPLETE;
	FenceKeepInConflict      = false;
	FenceKeepOutConflict     = false;
	TrafficConflict          = false;
	StandoffConflict         = false;	
	NominalPlan              = true;
	currentResolutionWP = 0;
	currentConflicts     = 0;
	joined = true;

	try{
	    FileReader in = new FileReader("params/icarous.txt");
	    SeparatedInput reader = new SeparatedInput(in);

	    reader.readLine();
	    ParameterData parameters = reader.getParametersRef();
	    
	    resolutionSpeed   = (float) parameters.getValue("ResolutionSpeed");
	    gridsize          = parameters.getValue("gridsize");
	    buffer            = parameters.getValue("buffer");
	    lookahead         = parameters.getValue("lookahead");
	    proximityfactor   = parameters.getValue("proximityfactor");
	    standoff          = parameters.getValue("standoff");
	    XtrkDevGain       = parameters.getValue("XtrkDevGain") ;

	    if(XtrkDevGain < 0){
		XtrkDevGain = -XtrkDevGain;
	    }
	}
	catch(FileNotFoundException e){
	    System.out.println("parameter file not found");
	}
    }

    // Returns time corresponding current position in the resolution flight plan
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
	
	// Check for deviation from prescribed flight profile.
	CheckStandoff();
	    
	// Check for conflicts from DAIDALUS against other traffic.


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
	    
	switch(resolveState){

	case COMPUTE:
	    // Call the relevant resolution functions to resolve conflicts
	    // [TODO:] Add conflict resolution table to add prioritization/scheduling to handle multiple conflicts
	    
	    if(FenceKeepInConflict){
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
		ResolveStandoffConflict();
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

	double crossTrackDeviation = sgn*dist*Math.sin(Math.toRadians(bearing));

	if(Math.abs(crossTrackDeviation) > standoff){
	    StandoffConflict = true;	    
	    Conflict cf = new Conflict(PRIORITY_LEVEL.MEDIUM,CONFLICT_TYPE.FLIGHTPLAN);
	    Conflict.AddConflictToList(conflictList,cf);
	}else if(Math.abs(crossTrackDeviation) < (standoff)/3){
	    StandoffConflict = false;
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
	    SimplePoly expfence = GF.pu.bufferedConvexHull(GF.geoPolyLLA,GF.hthreshold,GF.vthreshold);
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

	if(StandoffConflict){
	    double Vs = XtrkDevGain*crossTrackDeviation;
	    double V  = UAS.GetSpeed();
	    
	    if(Math.pow(Math.abs(Vs),2) >= Math.pow(V,2)){
		Vs = V;
	    }

	    double Vf       = Math.sqrt( Math.pow(V,2) - Math.pow(Vs,2) );

	    Position PrevWP = FlightData.CurrentFlightPlan.point(FlightData.FP_nextWaypoint - 1).position();
	    Position NextWP = FlightData.CurrentFlightPlan.point(FlightData.FP_nextWaypoint).position();

	    double Trk = PrevWP.track(NextWP);

	    Vn = Math.cos(Trk) - Math.sin(Trk);
	    Ve = Math.sin(Trk) + Math.cos(Trk);
	    Vu = 0;

	    // System.out.format("Vn=%f,Ve=%f\n",Vn,Ve);
	    
	}
	else{
	    Vn = 0;
	    Ve = 0;
	    Vu = 0;
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

	    UAS.SetVelocity(Vn,Ve,Vu);

	    if(!StandoffConflict){
		executeState = EXECUTE_STATE.COMPLETE;
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
