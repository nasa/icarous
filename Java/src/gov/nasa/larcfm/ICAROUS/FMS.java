/**
 * Flight Management System (FMS)
 * Contact: Swee Balachandran (swee.balachandran@nianet.org)
 * 
 * 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.ICAROUS;
import com.MAVLink.enums.*;
import com.MAVLink.common.*;

public class FMS implements Runnable{

    private enum FMS_STATE{
	IDLE, PREFLIGHT, FLIGHT, TERMINATE
    }
    
    private Thread t;
    private String threadName;    
    private Aircraft UAS;
    
    private FMS_STATE state;
    private boolean FMSrunning;
    
    public FMS(String name,Aircraft ac){
	threadName       = name;
	UAS              = ac;
	FMSrunning       = true;
    }

    public void run(){	
	while(FMSrunning){	    
	    FlightManagement();
	}
    }
        
    public void start(){
	System.out.println("Starting "+threadName);
	t = new Thread(this,threadName);
	t.start();
    }
	
    public void FlightManagement(){

	int status = 0;
	
	switch(state){

	case IDLE:

	    /* - Wait for mission start flag from COMBOX */
	    
	    if(SharedData.startMission == 1){
		SharedData.startMission = -1;		
		state = FMS_STATE.START;
	    }
	    
	    break;
	    
	    
        case PREFLIGHT:

	    /* Peform preflight activities */	    
	    status = UAS.PreFlight();

	    if(status == 1){
		state = FMS_STATE.MISSION;
	    }
	    
	    break;
	    
	case FLIGHT:
	    
	    /* Start monitoring mission */
	    status = UAS.Flight();

	    if(status == 1){
		state = FMS_STATE.TERMINATE;
	    }
	    break;

	case TERMINATE:
	    /* Perform post flight activites */
	    
	    status = UAS.Terminate(); 
	    
	    if(status == 1){
		FMSrunning = false;
	    }	    
	    break;

	}	
    }

      
}

