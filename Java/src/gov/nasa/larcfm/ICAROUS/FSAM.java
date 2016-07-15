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

public class FSAM{

    Aircraft UAS;
    AircraftData FlightData;
    MAVLinkMessages Inbox;
    
    public FSAM(Aircraft ac){
	UAS = ac;
	FlightData = ac.FlightData;
	Inbox   = FlightData.Inbox;
    }

    public int Monitor(){

	FlightPlan FP       = FlightData.CurrentFlightPlan;
	Position currentPos = FlightData.currPosition;
	
	long current_time   = System.nanoTime();
	
	long time_elapsed   = current_time - timeEvent1;
	
	// Check for deviation from prescribed flight profile.

	// Check for conflicts from DAIDALUS against other traffic.

	// Check for geofence resolutions.
		
	// Check for mission payload related flags.

	// Check mission progress.
	if(time_elapsed > 5E9){
	    timeEvent1 = current_time;    
	    Waypoint wp = FP.GetWaypoint(FP.nextWaypoint);	    
	    double dist[] = FP.Distance2Waypoint(currentPos,wp.pos);	    
	    System.out.format("Distance to next waypoint: %2.2f (Miles), heading: %3.2f (degrees)\n",dist[0]*0.62,dist[1]);	    
	}
	
	if(CheckMissionItemReached()){
	    System.out.println("Reached waypoint");
	    FlightData.CurrentFlightPlan.nextWaypoint++;
	}
	

	// Determine mode
	reqMode = AP_MODE.AUTO;

	
    }

    public int Resolve(){


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
