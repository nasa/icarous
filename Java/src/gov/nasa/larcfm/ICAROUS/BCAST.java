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
    public Aircraft UAS;
    public AircraftData FlightData;
    public Interface Intf;
    
    public BCAST(String name,Aircraft uas,Interface bcastIntf){
	threadName       = name;
	UAS              = uas;               
	FlightData       = UAS.FlightData;
	Intf             = bcastIntf;
    }

    public void run(){

	msg_heartbeat_icarous ICAROUSstate = new msg_heartbeat_icarous();

	while(true){	    	    	    	   


	    ICAROUSstate.status = (byte) UAS.GetAircraftState();
	    
	    // Broadcast messages here
	    Intf.Write(ICAROUSstate);
	    Intf.Write(FlightData.Inbox.GlobalPositionInt());


	}
    }

    public void start(){
	System.out.println("Starting "+threadName);
	t = new Thread(this,threadName);
	t.start();
    }
    
}
