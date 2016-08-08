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

    COMPUTE, EXECUTE, MONITOR, RESUME, NOOP
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
    
    private int currentResolutionLeg;

    public RESOLVE_STATE rState;
    public EXECUTE_STATE executeState;

    private boolean FenceKeepInConflict;
    private boolean FenceKeepOutConflict;
    private boolean TrafficConflict;
    private boolean FlightPlanConflict;
    private boolean GotoNextWP;
    
    private float resolutionSpeed;
    private int currentConflicts;
    private boolean NominalPlan;

    private double gridsize;
    private double buffer;
    private double lookahead;
    private double proximityfactor;
    
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
	rState = RESOLVE_STATE.NOOP;
	executeState = EXECUTE_STATE.COMPLETE;
	FenceKeepInConflict   = false;
	FenceKeepOutConflict  = false;
	TrafficConflict          = false;
	FlightPlanConflict       = false;
	NominalPlan              = true;
	currentResolutionLeg = 0;
	currentConflicts     = 0;

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
	}
	catch(FileNotFoundException e){
	    System.out.println("parameter file not found");
	}
    }

    public double GetResolutionTime(){
	Plan FP = ResolutionPlan;

	
	if(currentResolutionLeg < ResolutionPlan.size()-1){
	    Position pos = FlightData.acState.positionLast();
	    double legDistance    = FP.pathDistance(currentResolutionLeg);
	    double legTime        = FP.getTime(currentResolutionLeg + 1) - FP.getTime(currentResolutionLeg);
	    double lastWPDistance = FP.point(currentResolutionLeg).position().distanceH(pos);
	    double currentTime    = FP.getTime(currentResolutionLeg) + legTime/legDistance * lastWPDistance;
	
	    return currentTime;
	}
	else{
	    return ResolutionPlan.getLastTime();
	}
    }

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
	

	UAS.FlightData.getPlanTime();
	
	// Check for geofence resolutions.
	CheckGeoFences();
	
	// Check for deviation from prescribed flight profile.

	    
	// Check for conflicts from DAIDALUS against other traffic.


	// Check mission progress.
	if(timeElapsed > 5E9){
	    timeEvent1 = timeCurrent;      
	    //System.out.format("Distance to next waypoint: %2.2f \n",distance);	   
	}
	
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
	
	if(conflictList.size() > 0){

	    if(conflictList.size() != currentConflicts){
		currentConflicts = conflictList.size();
		rState = RESOLVE_STATE.COMPUTE;
		UAS.error.addWarning("[" + UAS.timeLog + "] MSG: Conflict(s) detected");
	    }	    	    
	    
	    return FSAM_OUTPUT.CONFLICT;	    
	}
	else
	    return FSAM_OUTPUT.NOOP;
	
    }

    public int Resolve(){

	int status;
	Plan CurrentFP  = FlightData.CurrentFlightPlan;
	boolean UsePlan = true;
	    
	switch(rState){

	case COMPUTE:
	    
	    if(FenceKeepInConflict){
		UAS.SetSpeed(resolutionSpeed);
		ResolveKeepInConflict();		
	    }

	    if(FenceKeepOutConflict){
	       UAS.SetSpeed(resolutionSpeed);
	       ResolveKeepOutConflict();		
	    }

	    if(UsePlan){
		rState       = RESOLVE_STATE.EXECUTE;
		executeState = EXECUTE_STATE.START;
		NominalPlan  = false;
	    }
	    else{

	    }
	    
	    break;

	case EXECUTE:
	   
	    if( executeState != EXECUTE_STATE.COMPLETE ){
		ExecuteResolution();
	    }
	    else{
		rState = RESOLVE_STATE.RESUME;
	    }
	    
	    break;

	case RESUME:

	    Iterator Itr = conflictList.iterator();
	    while(Itr.hasNext()){
		Conflict cf = (Conflict) Itr.next();
		Itr.remove();
	    }
	    currentConflicts = conflictList.size();
	    
	    msg_mission_set_current msgMission = new msg_mission_set_current();
	    msgMission.target_system     = 0;
	    msgMission.target_component  = 0;
	    msgMission.seq               = FlightData.FP_nextWaypoint;	    		
	    UAS.error.addWarning("[" + UAS.timeLog + "] CMD: Set next mission item");
	    UAS.apIntf.Write(msgMission);

	    UAS.apMode = Aircraft.AP_MODE.AUTO;
	    UAS.SetMode(3);
	    UAS.error.addWarning("[" + UAS.timeLog + "] MODE: AUTO");
	    rState = RESOLVE_STATE.NOOP;
	    NominalPlan = true;
	    
	    break;

	case NOOP:

	    break;
	    
	}
	
	return 0;
    }

    public boolean CheckMissionItemReached(){

	boolean reached = false;
	
	if(Inbox.UnreadMissionItemReached()){
	    Inbox.ReadMissionItemReached();
	    reached = true;
	}


	return reached;	
    }

    public boolean CheckAPHeartBeat(){

	FlightData.Inbox.ReadHeartbeat_AP();
		
	apIntf.Read();
			
	if(FlightData.Inbox.UnreadHeartbeat_AP()){
	    return true;
	}
	else{
	    return false;
	}
	
    }

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
	    GF.CheckViolation(FlightData.acState.positionLast(),planTime,CurrentPlan);

	    Conflict cf;	   

	    if(GF.conflict){
		
		if(GF.Type == 0){
		    cf = new Conflict(PRIORITY_LEVEL.MEDIUM,CONFLICT_TYPE.KEEP_IN,GF);
		    FenceKeepInConflict = true;
		}
		else{
		    cf = new Conflict(PRIORITY_LEVEL.MEDIUM,CONFLICT_TYPE.KEEP_OUT,GF);
		    FenceKeepOutConflict = true;
		}
		Conflict.AddConflictToList(conflictList,cf);
		
	    }
	}
	
    }

    public void ResolveKeepInConflict(){

	Plan CurrentFP = FlightData.CurrentFlightPlan;
	GeoFence GF = conflictList.get(0).fence;		
	NavPoint wp = new NavPoint(GF.SafetyPoint,0);
	ResolutionPlan.clear();
	ResolutionPlan.add(wp);
	
	NavPoint nextWP = CurrentFP.point(FlightData.FP_nextWaypoint);
	if(!GF.CheckWaypointFeasibility(wp.position(),nextWP.position())){
	    GotoNextWP = true;
	    FlightData.FP_nextWaypoint++;				
	}else{
	    GotoNextWP = false;
	}
	

    }

    public void ResolveKeepOutConflict(){

	Plan CurrentFP = FlightData.CurrentFlightPlan;

	double minTime = Double.MAX_VALUE;
	double maxTime = 0.0;

	double currentTime = UAS.FlightData.planTime;

	ArrayList<PolyPath> LPP = new ArrayList<PolyPath>();
	ArrayList<PolyPath> CPP = new ArrayList<PolyPath>();
	WeatherUtil WU = new WeatherUtil();
	
	// Get conflict start and end time
	for(int i=0;i<conflictList.size();i++){

	    GeoFence GF = (GeoFence) conflictList.get(i).fence;
	    
	    if(GF.entryTime <= minTime){
		minTime = GF.entryTime;
	    }
	    
	    if(GF.exitTime >= maxTime){
		maxTime = GF.exitTime;
	    }
	    
	    LPP.add(GF.geoPolyPath);
	    
	}

	CPP.add(FlightData.fenceList.get(0).geoPolyPath);
	
	minTime = minTime - lookahead;
	maxTime = maxTime + lookahead;
	
	if(minTime < currentTime){
	    minTime = currentTime;
	}

	FlightData.FP_nextWaypoint = CurrentFP.getSegment(maxTime)+1;	

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

	// Instantiate a grid to search over
	DensityGrid dg = new DensityGrid(BR,start,end,(int)buffer,gridsize,true);
	dg.snapToStart();
	
	// Set weights for the search space and obstacles
	dg.setWeights(5.0);

	for(int i=1;i<FlightData.fenceList.size();i++){
	    SimplePoly GF = FlightData.fenceList.get(i).geoPolyLLA;
	    dg.setWeightsInside(GF,100.0);
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
	double ETA   = 0.0;
	ResolutionPlan.add(new NavPoint(PlanPosition.get(0),ETA));
	for(int i=1;i<PlanPosition.size();i++){
	    Position pos = PlanPosition.get(i);
	    double distance = pos.distanceH(PlanPosition.get(i-1));
	    ETA      = ETA + distance/resolutionSpeed;

	    ResolutionPlan.add(new NavPoint(pos,ETA));
	}

	
	
    }

    
    public void ExecuteResolution(){	
	
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
	    
	    currentResolutionLeg = 0;
	    executeState = EXECUTE_STATE.SEND_COMMAND;
	    break;
	    
	case SEND_COMMAND:

	    NavPoint wp = ResolutionPlan.point(currentResolutionLeg);
	    UAS.SetGPSPos(wp.lla().latitude(),wp.lla().longitude(),wp.alt());
	    executeState = EXECUTE_STATE.AWAIT_COMPLETION;
	    break;
	    
	case AWAIT_COMPLETION:
	    
	    Position pos   = ResolutionPlan.point(currentResolutionLeg).position();
	    double dist    = pos.distanceH(UAS.FlightData.acState.positionLast());

	    if(dist < 1){

		currentResolutionLeg = currentResolutionLeg + 1;
		if(currentResolutionLeg < ResolutionPlan.size()){
		    executeState = EXECUTE_STATE.SEND_COMMAND;
		}
		else{
		    ResolutionPlan.clear();
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
