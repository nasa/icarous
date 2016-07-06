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
import com.MAVLink.enums.*;
import com.MAVLink.common.*;

public class FMS_Thread extends Aircraft implements Runnable{

    public Thread t;
    public String threadName;    
    
    public FMS_Thread(String name,AircraftData Input, ICAROUS_Interface apInterface){
	threadName       = name;
	SharedData       = Input;
	Intf             = apInterface;
	apState          = new AircraftData(AircraftData.NO_MESSAGES);
    }

    public void run(){

	while(true){
	    synchronized(SharedData){
		SharedData.GetDataFromMessages();
		apState.CopyAircraftStateInfo(SharedData);
	    }
	    
	    this.FlightManagement();
	}
    }
    
    
    public void start(){
	System.out.println("Starting "+threadName);
	t = new Thread(this,threadName);
	t.start();
    }

	
    public void FlightManagement(){
	
	switch(fmsState){

	case IDLE:

	    /* - Wait for mission start flag from COMBOX 
	       - Set current flight plan as the new flight plan 
	       - Send the current flight plan to the pixhawk    */
	    
	    synchronized(SharedData){
		if(SharedData.startMission == 1){
		    SharedData.startMission = -1;
		    SharedData.CurrentFlightPlan.Copy(SharedData.NewFlightPlan);
		    this.SendFlightPlanToAP();
		    fmsState = QUAD_FMS.MISSION;
		}
	    }
	    
	    break;

	case MISSION:

	    /* Start monitoring mission */
	    Mission();
	    
	    break;

	case TERMINATE:

	    /* Perform termination functions */
	    break;

	}	
    }

      
}

