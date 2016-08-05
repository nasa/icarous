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
import com.MAVLink.common.*;
import com.MAVLink.enums.*;

import gov.nasa.larcfm.Util.Plan;
import gov.nasa.larcfm.Util.AircraftState;
import gov.nasa.larcfm.Util.Position;
import gov.nasa.larcfm.Util.NavPoint;

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
	

	UAS.FlightData.GetPlanTime();
	
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
		UAS.SetSpeed(ResolutionSpeed);
		ResolveKeepInConflict();		
	    }

	    if(FenceKeepOutConflict){
	       UAS.SetSpeed(ResolutionSpeed);
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
	    
	    if(GotoNextWP){
		msg_mission_set_current msgMission = new msg_mission_set_current();
		msgMission.target_system     = 0;
		msgMission.target_component  = 0;

		FlightData.FP_nextWaypoint++;
		msgMission.seq               = FlightData.FP_nextWaypoint;	    
		
		UAS.error.addWarning("[" + UAS.timeLog + "] CMD: Set next mission item");
		UAS.apIntf.Write(msgMission);
		
	    }

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
	
	for(int i=0;i< FlightData.fenceList.size();i++){
	    GeoFence GF = (GeoFence) FlightData.fenceList.get(i);
	    GF.CheckViolation(FlightData.acState.positionLast(),UAS.FlightData.planTime,UAS.FlightData.CurrentFlightPlan);

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
	
	// Get conflict start and end time
	for(int i=0;i<Conflict.size();i++){

	    Conflict conf = (Conflict) conflictList.get(i);

	    if(conf.conflictType == CONFLICT_TYPE.KEEP_OUT){

		if(GF.EntryTime <= minTime){
		    minTime = GF.entryTime;
		}

		if(GF.ExitTime >= maxTime){
		    maxTime = GF.exitTime;
		}
   				
		LPP.add(GF.geoPolyPath);
	    }
	}

	CPP = add(FlightData.fenceList.get(0));
	
	minTime = minTime - 3;
	maxTime = maxTime + 3;
	
	if(minTime < currentTime){
	    minTime = currentTime;
	}

	// Get flight plan between start time and end time (with a 3 second buffer on both sides)
	Plan ConflictPlan = PlanUtil.cutDown(CurrentFP,minTime,maxTime);

	
	// Reroute flight plan
	UAS.setMode(4); // Set mode to guided for quadrotor to hover before replanning
	
	Pair<Plan,DensityGrid> Resolution = WU.reRouteWx(ConflictFP,LPP,gridsize,buffer,0,lookahead,null,false,false,
							 currentTime,0.0,true,0.0,0.0,false);

	// Smooth flight plan
	ResolutionPlan  = SmoothPlan(Resolution.getFirst());
	
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

    public static Plan SmoothPlan(Plan input){

	int size = input.size();
	int filtersize = 3;

	if(filtersize >= size){
	    return input;
	}

	Plan PF = new Plan();
	PF.add(input.point(0));
	PF.setTime(0,0);
	
	for(int i=0;i<size;i++){

	    double cumlat = 0;
	    double cumlon = 0;
	    double cumalt = 0;
	    
	    for(int j=0;j<filtersize;j++){

		if(i+j < size){
		    cumlat = cumlat + input.point(i+j).position().latitude();
		    cumlon = cumlon + input.point(i+j).position().longitude();
		    cumalt = cumalt + input.point(i+j).position().alt();
		}
		else{
		    cumlat = cumlat + input.point(size-1).position().latitude();
		    cumlon = cumlon + input.point(size-1).position().longitude();
		    cumalt = cumalt + input.point(size-1).position().alt();
		}
	    }

	    Position p =Position.makeLatLonAlt(cumlat/filtersize,"degree",
					       cumlon/filtersize,"degree",
					       cumalt/filtersize,"m");


	    // Plan already has the first waypoint hence the i instead of i-1.
	    double time = PF.getTime(i) + p.distanceH(PF.point(i).position())/resolutionSpeed;
	    NavPoint np = new NavPoint(p,time);
	    PF.add(np);

	    
	}
		    
	return PF;

    }
    
}
