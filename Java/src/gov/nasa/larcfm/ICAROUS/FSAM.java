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

import gov.nasa.larcfm.Util.Plan;
import gov.nasa.larcfm.Util.AircraftState;
import gov.nasa.larcfm.Util.Position;
import gov.nasa.larcfm.Util.NavPoint;

enum FSAM_OUTPUT{
    CONFLICT,NOOP, TERMINATE
}

enum RESOLVE_STATE{

    COMPUTE, EXECUTE, MONITOR, JOIN, NOOP
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
	int currentResolutionLeg = 0;
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
	    UAS.error.addWarning("[" + UAS.timeLog + "] MSG: Reached waypoint");
	    FlightData.FP_nextWaypoint++;	  
	}
	
	if(conflictList.size() > 0){
	    if(rState == RESOLVE_STATE.NOOP){
		UAS.error.addWarning("[" + UAS.timeLog + "] MSG: Conflict(s) detected");
		rState = RESOLVE_STATE.COMPUTE;
	    }
	    
	    return FSAM_OUTPUT.CONFLICT;	    
	}
	else
	    return FSAM_OUTPUT.NOOP;
	
    }

    public int Resolve(){

	int status;
	Plan CurrentFP = FlightData.CurrentFlightPlan;
	
	switch(rState){

	case COMPUTE:
	    
	    if(FenceKeepInConflict){
		ResolveKeepInConflict();		
	    }
	    
	    rState       = RESOLVE_STATE.EXECUTE;
	    executeState = EXECUTE_STATE.START;
	    
	    break;

	case EXECUTE:

	    if( executeState != EXECUTE_STATE.COMPLETE ){
		ExecuteResolution();
	    }
	    else{
		rState = RESOLVE_STATE.JOIN;
	    }
	    
	    break;

	case JOIN:


	    Iterator Itr = conflictList.iterator();
	    while(Itr.hasNext()){
		Conflict cf = (Conflict) Itr.next();
		Itr.remove();
	    }
	    
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
	    GF.CheckViolation(FlightData.acState.positionLast());

	    Conflict cf;	   

	    if(GF.hconflict || GF.vconflict){
		
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
	
	ResolutionPlan.add(wp);
	
	NavPoint nextWP = CurrentFP.point(FlightData.FP_nextWaypoint);
	if(!GF.CheckWaypointFeasibility(wp.position(),nextWP.position())){
	    GotoNextWP = true;	
	}else{
	    GotoNextWP = false;
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
		}
	    }
	    
	    break;
	    
	    
	}//end switch case


    }//end function

    
}
