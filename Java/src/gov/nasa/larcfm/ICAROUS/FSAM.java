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

enum FSAM_OUTPUT{
	CONFLICT,NOOP
}

enum RESOLVE_STATE{

    COMPUTE, EXECUTE, MONITOR, JOIN, NOOP
}

enum EXECUTE_STATE{
    START, SEND_COMMAND, AWAIT_COMPLETION, COMPLETE
}

public class FSAM{

    Aircraft UAS;
    AircraftData FlightData;
    MAVLinkMessages Inbox;
    Mission mission;
    Interface apIntf;
    double distance2WP;
    double heading2WP;

    long timeEvent1;
    long timeElapsed;
    FSAM_OUTPUT output;

    List<Conflict> conflictList;
    List<Waypoint> ResolutionPlan;
    int currentResolutionLeg;
    RESOLVE_STATE rState;
    EXECUTE_STATE executeState;

    boolean FenceKeepInConflict;
    boolean FenceKeepOutConflict;
    boolean TrafficConflict;
    boolean FlightPlanConflict;
    boolean GotoNextWP;
    
    public FSAM(Aircraft ac,Mission ms){
	UAS = ac;
	FlightData = ac.FlightData;
	Inbox   = FlightData.Inbox;
	mission = ms;
	timeEvent1 = 0;
	timeElapsed = 0;
	apIntf = ac.apIntf;
	conflictList   = new ArrayList<Conflict>();
	ResolutionPlan = new ArrayList<Waypoint>();
	rState = RESOLVE_STATE.NOOP;
	executeState = EXECUTE_STATE.COMPLETE;
	FenceKeepInConflict   = false;
	FenceKeepOutConflict  = false;
	TrafficConflict          = false;
	FlightPlanConflict       = false;
	int currentResolutionLeg = 0;
    }

    public FSAM_OUTPUT Monitor(){

	FlightPlan FP       = FlightData.CurrentFlightPlan;
	Position currentPos = FlightData.currPosition;

	
	long timeCurrent    = UAS.timeCurrent;
	timeElapsed         = timeCurrent - timeEvent1;
	
	if(timeEvent1 == 0){
	    timeEvent1 = UAS.timeStart;
	}

	//Get distance to next waypoint
	Waypoint wp = FP.GetWaypoint(FP.nextWaypoint);	    
	double dist[] = FP.Distance2Waypoint(currentPos,wp.pos);
	distance2WP = dist[0]*1000;
	heading2WP  = dist[1];

	// Check for geofence resolutions.
	CheckGeoFences();
	
	// Check for deviation from prescribed flight profile.

	    
	// Check for conflicts from DAIDALUS against other traffic.


	// Check mission progress.
	if(timeElapsed > 5E9){
	    timeEvent1 = timeCurrent;      
	    //System.out.format("Distance to next waypoint: %2.2f (Miles), heading: %3.2f (degrees)\n",dist[0]*0.62,dist[1]);	   
	}
	
	if(CheckMissionItemReached()){
	    System.out.println("Reached waypoint");
	    FlightData.CurrentFlightPlan.nextWaypoint++;
	}
	
	if(conflictList.size() > 0){
	    if(rState == RESOLVE_STATE.NOOP){
		rState = RESOLVE_STATE.COMPUTE;
	    }
	    
	    return FSAM_OUTPUT.CONFLICT;	    
	}
	else
	    return FSAM_OUTPUT.NOOP;
	
    }

    public int Resolve(){

	int status;
	FlightPlan CurrentFP = FlightData.CurrentFlightPlan;
	
	switch(rState){

	case COMPUTE:
	    
	    if(FenceKeepInConflict){
		GeoFence GF = conflictList.get(0).fence;		
		Waypoint wp = new Waypoint(0,GF.SafetyPoint.lat,GF.SafetyPoint.lon,GF.SafetyPoint.alt_msl,400.0f);
		ResolutionPlan.add(wp);

		Waypoint nextwp = CurrentFP.GetWaypoint(CurrentFP.nextWaypoint);
		if(!GF.CheckWaypointFeasibility(wp.pos,nextwp.pos)){
		    GotoNextWP = true;
		    System.out.println("Goto next waypoint after resolution");
		}else{
		    GotoNextWP = false;
		}
		
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
	    
	    System.out.println("Remove conflicts:"+conflictList.size());

	    if(GotoNextWP){
		msg_mission_set_current msgMission = new msg_mission_set_current();
		msgMission.target_system     = 0;
		msgMission.target_component  = 0;
		msgMission.seq               = 3;	    

		
		UAS.apIntf.Write(msgMission);
	    }
	    
	    UAS.SetMode(3);

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
	
	apIntf.SetTimeout(5000);
	FlightData.Inbox.decode_message(apIntf.Read());
	apIntf.SetTimeout(0);
		

	if(FlightData.Inbox.UnreadHeartbeat_AP()){
	    return true;
	}
	else{
	    return false;
	}
	
    }

    public void CheckGeoFences(){

	for(int i=0;i< FlightData.fenceList.size();i++){
	    GeoFence GF = (GeoFence) FlightData.fenceList.get(i);
	    GF.CheckViolation(FlightData.currPosition);

	    Conflict cf;

	    FenceKeepInConflict  = false;
	    FenceKeepOutConflict = false;

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


    public void ExecuteResolution(){

	Waypoint wp;
	
	switch(executeState){

	case START:
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

	    wp = ResolutionPlan.get(currentResolutionLeg);
	    UAS.SetGPSPos(wp.pos.lat,wp.pos.lon,wp.pos.alt_msl);
	    executeState = EXECUTE_STATE.AWAIT_COMPLETION;
	    break;
	    
	case AWAIT_COMPLETION:
	    
	    wp = ResolutionPlan.get(currentResolutionLeg);
	    Position pos   = new Position(wp.pos.lat,wp.pos.lon,wp.pos.alt_msl);
	    double dist[]  = FlightPlan.Distance2Waypoint(UAS.FlightData.currPosition,pos);

	    if(dist[0]*1000 < 1){
		System.out.println("Reached safe position");
		currentResolutionLeg = currentResolutionLeg + 1;
		if(currentResolutionLeg < ResolutionPlan.size()){
		    executeState = EXECUTE_STATE.SEND_COMMAND;
		}
		else{
		    executeState = EXECUTE_STATE.COMPLETE;
		}
	    }
	    
	    break;
	    
	    
	case COMPLETE:

	    Iterator Itr = ResolutionPlan.iterator();
	    while(Itr.hasNext()){
		Waypoint wpr = (Waypoint) Itr.next();
		Itr.remove();
	    }

	    
	    break;
	}//end switch case


    }//end function

    
}
