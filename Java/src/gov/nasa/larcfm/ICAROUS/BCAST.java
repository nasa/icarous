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

public class BCAST implements Runnable{

    public Thread t;
    public String threadName;
    public AircraftData FlightData;
    public Interface Intf;
    
    public BCAST(String name,AircraftData acData, Interface bcastInterface){
	threadName       = name;
	FlightData       = acData;
	Intf             = bcastInterface;
    }

    public void run(){

	
	while(true){
	
	    // Broadcast messages here	  
	    // Intf.Write(FlightData.Inbox.msgHeartbeat);
	    	  
	}
    }

    public void start(){
	System.out.println("Starting "+threadName);
	t = new Thread(this,threadName);
	t.start();
    }
    
}
