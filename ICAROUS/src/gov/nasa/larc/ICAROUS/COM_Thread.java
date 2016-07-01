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
package gov.nasa.larc.ICAROUS;


public class COM_Thread implements Runnable{

    public Thread t;
    public String threadName;
    public AircraftData SharedData;
    public ICAROUS_Interface COM;
    
    public COM_Thread(String name,AircraftData Input, ICAROUS_Interface comInterface){
	threadName       = name;
	SharedData       = Input;
	COM              = comInterface;
    }

    public void run(){

	COM.Read();

	if(SharedData.RcvdMessages.FlightPlanUpdateInterrupt == 1){
	    SharedData.RcvdMessages.FlightPlanUpdateInterrupt = 0;
	
	}

	if(SharedData.RcvdMessages.GeoFenceUpdateInterrupt == 1){
	    SharedData.RcvdMessages.GeoFenceUpdateInterrupt = 0;

	}

	if(SharedData.RcvdMessages.TrafficUpdateInterrupt == 1){
	    SharedData.RcvdMessages.TrafficUpdateInterrupt = 0;

	}

	if(SharedData.RcvdMessages.ObstacleUpdateInterrupt == 1){
	    SharedData.RcvdMessages.ObstacleUpdateInterrupt = 0;

	}

	// TODO: Implement COM box write
	// COM.Write()	

    }

    
    public void start(){
	System.out.println("Starting "+threadName);
	t = new Thread(this,threadName);
	t.start();
    }

	
    

    
}
