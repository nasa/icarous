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

public class FMS_Thread implements Runnable{

    public Thread t;
    public String threadName;
    public AircraftData SharedData;
    public AircraftData apState;
    public ICAROUS_Interface AP;
    
    public FMS_Thread(String name,AircraftData Input, ICAROUS_Interface apInterface){
	threadName       = name;
	SharedData       = Input;
	AP               = apInterface;
	apState          = new AircraftData(AircraftData.NO_MESSAGES);
    }

    public void run(){
		
	synchronized(SharedData){
	    SharedData.GetDataFromMessages();
	    apState.CopyAircraftStateInfo(SharedData);
	}
	
	this.FlightManagement();
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
	AP.SetMode(4);
	
	// Arm the throttles
	AP.SendCommand(0,0,0,MAV_CMD.MAV_CMD_COMPONENT_ARM_DISARM,
		       1,0,0,0,0,0,0);

	// Takeoff at current location
	AP.SendCommand(0,0,0,MAV_CMD.MAV_CMD_NAV_TAKEOFF,
		       1,0,0,0, (float) apState.aircraftPosition.lat,
		                (float) apState.aircraftPosition.lon,
		                (float) apState.aircraftPosition.alt + 10.0f);
    }

    
}
