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

public class DAQ implements Runnable{

    public Thread t;
    public String threadName;
    public AircraftData sharedData;
    public Interface icarousAP;
    
    public DAQ(String name,AircraftData acData, Interface apIntf){
	threadName   = name;
	sharedData   = acData;
	icarousAP    = apIntf;	
    }

    public void run(){
	while(true){
	    icarousAP.Read();   
	}
    }

    public void start(){
	System.out.println("Starting "+threadName);
	t = new Thread(this,threadName);
	t.start();
    }

}
