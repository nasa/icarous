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
    RESOLVE_STATE rState;
    
    public FSAM(Aircraft ac,Mission ms){
	UAS = ac;
	FlightData = ac.FlightData;
	Inbox   = FlightData.Inbox;
	mission = ms;
	timeEvent1 = 0;
	timeElapsed = 0;
	apIntf = ac.apIntf;
	conflictList = new ArrayList<Conflict>();
	rState = RESOLVE_STATE.COMPUTE;
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
	    return FSAM_OUTPUT.CONFLICT;	    
	}
	else
	    return FSAM_OUTPUT.NOOP;
	
    }

    public int Resolve(){

	switch(rState){

	case COMPUTE:
	    

	    rState = RESOLVE_STATE.EXECUTE;
	    
	    break;

	case EXECUTE:

	   	    
	    // Compute resolution
	    UAS.SetMode(4);

	    UAS.SetYaw(270.0);

	    try{
		Thread.sleep(3000);
	    }catch(InterruptedException e){
		System.out.println(e);
	    }	    

	    System.out.println("Setting safe position");
	    UAS.SetGPSPos(37.615288,-122.360266,20);
	    
	    	    
	    rState = RESOLVE_STATE.MONITOR;

	    break;

	case MONITOR:

	    FlightPlan FP  = FlightData.CurrentFlightPlan;
	    Position pos   = new Position(37.615288f,-122.360266f,20f);
	    double dist[]  = FP.Distance2Waypoint(UAS.FlightData.currPosition,pos);

	    if(dist[0] < 0.05){
		System.out.println("Reached safe position");
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
	    
	    msg_mission_set_current msgMission = new msg_mission_set_current();
	    msgMission.target_system = 0;
	    msgMission.target_component = 0;
	    msgMission.seq               = 3;	    

	    
	    UAS.apIntf.Write(msgMission);
	    UAS.SetMode(3);
	    
	    break;

	case NOOP:

	    break;
	    
	    // Execute resolution
	    
	    // Rejoin mission
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
	    
	    if(GF.hconflict || GF.vconflict){
		
		if(GF.Type == 0){
		    cf = new Conflict(PRIORITY_LEVEL.MEDIUM,CONFLICT_TYPE.KEEP_IN,GF);
		}
		else{
		    cf = new Conflict(PRIORITY_LEVEL.MEDIUM,CONFLICT_TYPE.KEEP_OUT,GF);
		}
		Conflict.AddConflictToList(conflictList,cf);
	    }
	}
	
    }
    
}
