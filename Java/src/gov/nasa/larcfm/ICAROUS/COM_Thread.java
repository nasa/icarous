/**
 * ICAROUS Interface
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


public class COM_Thread extends Aircraft implements Runnable{

    public Thread t;
    public String threadName;
        
    public COM_Thread(String name,AircraftData Input, ICAROUS_Interface comInterface){
	threadName       = name;
	SharedData       = Input;
	Intf             = comInterface;
    }

    public void run(){

	while(true){
	
	    Intf.Read();

	    // Handle new flight plan inputs
	    if(SharedData.RcvdMessages.FlightPlanUpdateInterrupt == 1){
		synchronized(SharedData){
		    SharedData.RcvdMessages.FlightPlanUpdateInterrupt = 0;
		    UpdateFlightPlan();
		}
	    }

	    // Handle geo fence messages
	    if(SharedData.RcvdMessages.GeoFenceUpdateInterrupt == 1){


	    }

	    // Handle traffic information
	    if(SharedData.RcvdMessages.TrafficUpdateInterrupt == 1){

	    }

	    // Handle obstacle information
	    if(SharedData.RcvdMessages.ObstacleUpdateInterrupt == 1){


	    }

	    // Handling mission commands (start/stop)
	    if(SharedData.RcvdMessages.RcvdMissionStart == 1){

		synchronized(SharedData){
		    SharedData.startMission = SharedData.RcvdMessages.msgMissionStartStop.missionStart;
		    SharedData.RcvdMessages.RcvdMissionStart = -1;
		}
		System.out.println("Received mission start");
	     		
	    }

	  
	    // TODO: Implement COM box write
	    // COM.Write()	
	}
    }

    
    public void start(){
	System.out.println("Starting "+threadName);
	t = new Thread(this,threadName);
	t.start();
    }

	
    
    
    
}
