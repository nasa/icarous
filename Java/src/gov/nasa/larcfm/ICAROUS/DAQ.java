/**
 * Data acquisition (DAQ)
 * 
 * This class constantly reads the given interface and stores
 * the MAVLink message.
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

import com.MAVLink.*;

public class DAQ implements Runnable{

    public Thread t;
    public String threadName;
    public AircraftData FlightData;
    public Interface icarousAP;
    
    public DAQ(String name,Aircraft UAS){
	threadName   = name;
	FlightData   = UAS.FlightData;
	icarousAP    = UAS.apIntf;	
    }

    public void run(){
	while(true){
	    icarousAP.SetTimeout(1);
	    icarousAP.Read();
	    icarousAP.SetTimeout(0);
	}
    }

    public void start(){
	System.out.println("Starting "+threadName);
	t = new Thread(this,threadName);
	t.start();
    }



}
