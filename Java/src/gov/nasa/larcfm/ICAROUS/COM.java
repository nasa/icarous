/**
 * Communication module
 * This class enables listening and responding to requests from
 * other onboard applications.
 *
 * Contact: Swee Balachandran (swee.balachandran@nianet.org)
 * 
 * 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.ICAROUS;

import com.MAVLink.icarous.*;
import com.MAVLink.*;

public class COM implements Runnable{

    public Thread t;
    public String threadName;
    public AircraftData FlightData;
    public Interface com;
        
    public COM(String name,AircraftData acData, Interface comInterface){
	threadName       = name;
	FlightData       = acData;
	com              = comInterface;
    }

    public void run(){

	while(true){
	
	    Read();

	    // Handle new flight plan inputs
	    if(FlightData.Inbox.UnreadFlightPlanUpdate()){
		FlightData.Inbox.ReadFlightPlanUpdate();		
		FlightData.UpdateFlightPlan(com);
	    }

	    // Handle geo fence messages
	  
	    // Handle traffic information
	  
	    // Handle obstacle information
	  

	    // Handle other points of interest (mission related)
	  

	    // Handling mission commands (start/stop)
	    if(FlightData.Inbox.UnreadMissionStart()){
		FlightData.Inbox.ReadMissionStart();		
		FlightData.startMission = FlightData.Inbox.MissionStartStop().missionStart;				
		System.out.println("Received mission start");
	     		
	    }

	  
	}
    }

    
    public void start(){
	System.out.println("Starting "+threadName);
	t = new Thread(this,threadName);
	t.start();
    }

    public void Read(){

	MAVLinkPacket RcvdPacket = com.Read();
	FlightData.Inbox.decode_message(RcvdPacket);
	
    }
    
    
}
