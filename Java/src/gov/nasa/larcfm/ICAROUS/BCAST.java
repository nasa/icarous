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

public class BCAST_Thread extends Aircraft implements Runnable{

    public Thread t;
    public String threadName;
    public AircraftData SharedData;
    
    public BCAST_Thread(String name,AircraftData Input, ICAROUS_Interface bcastInterface){
	threadName       = name;
	SharedData       = Input;
	Intf             = bcastInterface;
    }

    public void run(){

	
	while(true){
	
	    // Broadcast messages here	  
	    Intf.Write(SharedData.RcvdMessages.msgHeartbeat);
	    	  
	}
    }

    public void start(){
	System.out.println("Starting "+threadName);
	t = new Thread(this,threadName);
	t.start();
    }
    
}
