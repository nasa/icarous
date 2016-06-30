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

public class DAQ_Thread implements Runnable{

    public Thread t;
    public String threadName;
    public AircraftData SharedData;
    public ICAROUS_Interface AP;
    
    public DAQ_Thread(String name,AircraftData Input, ICAROUS_Interface apInterface){
	threadName   = name;
	SharedData   = Input;
	AP           = apInterface;	
    }

    public void run(){
	while(true){
	    synchronized(SharedData){

		// Pause reading if writing commands to pixhawk
		if( ((AP.interfaceType == ICAROUS_Interface.PX4) ||
		     (AP.interfaceType == ICAROUS_Interface.SITL)) &&
		    SharedData.sendmsg){
		    try{
			SharedData.wait();
		    }
		    catch(InterruptedException e){
			System.out.println(e);
		    }
		}
		AP.AP_Read();
	    }
	}
    }

    public void start(){
	System.out.println("Starting "+threadName);
	t = new Thread(this,threadName);
	t.start();
    }

}
