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
import gov.nasa.larcfm.Util.ParameterData;

public class FMS implements Runnable{

    private enum FMS_STATE{
	IDLE, PREFLIGHT, FLIGHT, TERMINATE
    }
    
    private Thread t;
    private String threadName;    
    private Aircraft UAS;
    
    private FMS_STATE state;
    private boolean FMSrunning;
    private ParameterData pData;
    
    public FMS(String name,Aircraft ac,ParameterData pdata){
	threadName       = name;
	UAS              = ac;
	FMSrunning       = true;
	state            = FMS_STATE.IDLE;
	pData            = pdata;
    }

    public void run(){	
	while(FMSrunning){
	    UAS.timeCurrent            = System.nanoTime();
	    FlightManagement();
	}

	UAS.error.addWarning("[" + UAS.timeLog + "] MSG:FMS shutting down");
    }
        
    public void start(){
	System.out.println("Starting "+threadName);
	t = new Thread(this,threadName);
	t.start();
    }

    public synchronized boolean isFMSrunning(){
	return FMSrunning;
    }

    public synchronized FMS_STATE getFMSstate(){
	return state;
    }
	
    public void FlightManagement(){

	int status = 0;

	
	UAS.FlightData.GetGPSdata();
	UAS.FlightData.GetAttitude();
	
	switch(state){

	case IDLE:

	    /* - Wait for mission start flag from ground station */
	    
	    if(UAS.FlightData.startMission == 0){		
		if(UAS.FlightData.InputFlightPlan.size() > 0){
		    state = FMS_STATE.PREFLIGHT;
		}
		else{
		    UAS.error.addError("[" + UAS.timeLog + "] MSG: No flight plan loaded");
		}
		UAS.FlightData.startMission = -1;
	    }
	    else if( UAS.FlightData.startMission > 0 && UAS.FlightData.startMission < UAS.FlightData.InputFlightPlan.size()){		
		if(UAS.FlightData.InputFlightPlan.size() > 0){
		    state = FMS_STATE.FLIGHT;
		    UAS.state = Aircraft.FLIGHT_STATE.CRUISE;
		    UAS.FlightData.FP_nextWaypoint = UAS.FlightData.startMission;
		    UAS.error.addWarning("[" + UAS.timeLog + "] MSG: Starting mission to waypoint:"+UAS.FlightData.FP_nextWaypoint);
		}
		else{
		    UAS.error.addError("[" + UAS.timeLog + "] MSG: No flight plan loaded");
		}
		UAS.FlightData.startMission = -1;
	    }
	    
	    break;
	    
	    
        case PREFLIGHT:

	    /* Peform preflight activities */	    
	    status = UAS.PreFlight();

	    if(status == 1){
		state     = FMS_STATE.FLIGHT;
		UAS.state = Aircraft.FLIGHT_STATE.TAKEOFF;
	    }
	    
	    break;
	    
	case FLIGHT:
	    
	    /* Start monitoring mission */
	    status = UAS.Flight();

	    if(status == 1){
		state = FMS_STATE.TERMINATE;
	    }
	    else if(status == -1){
		state = FMS_STATE.IDLE;
		UAS.error.addError("[" + UAS.timeLog + "] MSG: FMS state IDLE");
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

