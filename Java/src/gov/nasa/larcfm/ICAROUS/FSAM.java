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

enum FSAM_OUTPUT{
	CONFLICT,NOOP
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
    
    public FSAM(Aircraft ac,Mission ms){
	UAS = ac;
	FlightData = ac.FlightData;
	Inbox   = FlightData.Inbox;
	mission = ms;
	timeEvent1 = 0;
	timeElapsed = 0;
	apIntf = ac.apIntf;
	conflictList = new ArrayList<Conflict>();
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
	
	// Check for deviation from prescribed flight profile.

	// Check for conflicts from DAIDALUS against other traffic.

	// Check for geofence resolutions.
	for(int i=0;i< FlightData.fenceList.size();i++){
	    GeoFence GF = (GeoFence) FlightData.fenceList.get(i);
	    GF.CheckViolation(currentPos);

	    	    
	    if(GF.hconflict || GF.vconflict){


	    }
	}
	
		
	// Check for mission payload related flags.

	// Check mission progress.
	if(timeElapsed > 5E9){
	    timeEvent1 = timeCurrent;      
	    //System.out.format("Distance to next waypoint: %2.2f (Miles), heading: %3.2f (degrees)\n",dist[0]*0.62,dist[1]);	   
	}
	
	if(CheckMissionItemReached()){
	    System.out.println("Reached waypoint");
	    FlightData.CurrentFlightPlan.nextWaypoint++;
	}
	

	return output;
	
    }

    public int Resolve(){


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
    
}
