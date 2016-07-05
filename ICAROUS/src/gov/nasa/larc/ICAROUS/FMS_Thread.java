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
    public int FMS_state;
    
    public FMS_Thread(String name,AircraftData Input, ICAROUS_Interface apInterface){
	threadName       = name;
	SharedData       = Input;
	Intf             = apInterface;
	apState          = new AircraftData(AircraftData.NO_MESSAGES);
	FMS_state        = 0;
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
	
	/*
	  ICAROUS functionalities go in here.
	  - use synchronized codeblocks to access shared data.
	  - use Px4.SendCommand(targetsystem, targetcomponent, confirmation, command, 
	  param1,...,param7)
	  to send commands to the autopilot.
	*/
	
	/* --------- Example Mission ---------- */
	
	// Set mode to guided (4 is the custom mode in the arudpilot code)

	/*
	AP.SetMode(4);
	
	// Arm the throttles
	AP.SendCommand(0,0,0,MAV_CMD.MAV_CMD_COMPONENT_ARM_DISARM,
		       1,0,0,0,0,0,0);

	// Takeoff at current location
	AP.SendCommand(0,0,0,MAV_CMD.MAV_CMD_NAV_TAKEOFF,
		       1,0,0,0, (float) apState.aircraftPosition.lat,
		                (float) apState.aircraftPosition.lon,
		                (float) apState.aircraftPosition.alt + 10.0f);
	*/

	
	if(FMS_state == 0){

	    //System.out.println("start mission:"+SharedData.startMission);
	    synchronized(SharedData){
		if(SharedData.startMission == 1){
		    FMS_state++;
		    SharedData.startMission = -1;
		    System.out.println("Starting mission sequence");
		}
	    }
	}
	else if(FMS_state == 1){
	    System.out.println("Starting write");
	    synchronized(SharedData){
		this.SendFlightPlanToAP();
	    }
	    FMS_state = 2;
	}
	else if(FMS_state == 2){
	    SetMode(4);
	
	    // Arm the throttles
	    SendCommand(0,0,0,MAV_CMD.MAV_CMD_COMPONENT_ARM_DISARM,
			   1,0,0,0,0,0,0);
	    
	    // Takeoff at current location
	    SendCommand(0,0,0,MAV_CMD.MAV_CMD_NAV_TAKEOFF,
			   1,0,0,0, (float) apState.aircraftPosition.lat,
			   (float) apState.aircraftPosition.lon,
			   (float) apState.aircraftPosition.alt_msl + 10.0f);
	    FMS_state++;
	}
	else if(FMS_state == 3){

	    if(apState.aircraftPosition.alt_agl > 10.0f){
		System.out.println("Reached commanded altitude for takeoff");
		//Set mode to auto
		SetMode(3);
		FMS_state = 4;
	    }
	}
    }


    
      
}

