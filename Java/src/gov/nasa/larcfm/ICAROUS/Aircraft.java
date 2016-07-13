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
import com.MAVLink.enums.*;
import com.MAVLink.common.*;
import com.MAVLink.icarous.*;

public class Aircraft{

    

    public enum QUAD_FMS{
	IDLE, MISSION, TERMINATE 
    }

    public enum FMS_MISSION{
	TAKEOFF, MONITOR, LAND
    }

    public enum AP_MODE{
	ND,AUTO,GUIDED
    }
    
    public AircraftData SharedData;
    public AircraftData apState;
    public ICAROUS_Interface Intf;
    public QUAD_FMS fmsState;
    public FMS_MISSION missionState;

    private boolean takeoff_cmd_sent;
    private long takeoff_start_time;
    private long time1;
    private AP_MODE apMode;    // Current AP mode
    private AP_MODE reqMode;   // ICAROUS requested mode
    
    public Aircraft(){
	fmsState         = QUAD_FMS.IDLE;
	missionState     = FMS_MISSION.TAKEOFF;
	takeoff_cmd_sent = false;
	apMode           = AP_MODE.ND;
	reqMode          = AP_MODE.ND;
    }


    public boolean CheckHeartBeat(){

	boolean pulse = false;

	int compID    = (int) Intf.componentType;
	
	synchronized(SharedData){
	    if(compID == 1)
		SharedData.RcvdMessages.RcvdHeartbeat_AP  = 0;
	    else if(compID == 2)
		SharedData.RcvdMessages.RcvdHeartbeat_COM = 0;
	}
	
	Intf.SetTimeout(5000);
	Intf.Read();
	Intf.SetTimeout(0);

	synchronized(SharedData){
	    if( (compID == 1) && SharedData.RcvdMessages.RcvdHeartbeat_AP == 1)
		pulse = true;
	    else if( (compID == 2) && SharedData.RcvdMessages.RcvdHeartbeat_COM == 1)
		pulse = true;
		
	}

	return pulse;
    }
    
    // Function to send commands to pixhawk
    public int SendCommand( int target_system,
			    int target_component,
			    int command,
			    int confirmation,
			    float param1,
			    float param2,
			    float param3,
			    float param4,
			    float param5,
			    float param6,
			    float param7){

	msg_command_long CommandLong  = new msg_command_long();
	
	CommandLong.target_system     = (short) target_system;
	CommandLong.target_component  = (short) target_component;
	CommandLong.command           = command;
	CommandLong.confirmation      = (short) confirmation;
	CommandLong.param1            = param1;
	CommandLong.param2            = param2;
	CommandLong.param3            = param3;
	CommandLong.param4            = param4;
	CommandLong.param5            = param5;
	CommandLong.param6            = param6;
	CommandLong.param7            = param7;
	
	Intf.Write(CommandLong);

	try{
	    Thread.sleep(100);
	}catch(InterruptedException e){
	    System.out.println(e);
	}

	return CheckAcknowledgement();
    }

    // Function to set mode
    public int SetMode(int modeid){

	msg_set_mode Mode = new msg_set_mode();
	Mode.target_system = (short) 0;
	Mode.base_mode     = (short) 1;
	Mode.custom_mode   = (long) modeid;

	Intf.Write(Mode);

	try{
	    Thread.sleep(100);
	}catch(InterruptedException e){
	    System.out.println(e);
	}
	
	return CheckAcknowledgement();	
    }

    // Check acknowledgement
    public int CheckAcknowledgement(){

	synchronized(SharedData){
	    if(SharedData.RcvdMessages.msgCommandAck.result == 0 ){
		return 1;
	    }
	    else{
		System.out.println("Command not accepted\n");
		return 0;
	    }
	}
	
    }

    
    // Main function that runs the mission
    public void Mission(){

	FlightPlan FP       = SharedData.CurrentFlightPlan;
	Position currentPos = apState.aircraftPosition;
	long current_time   = System.nanoTime();
	long time_elapsed;
	
	switch(missionState){

	case TAKEOFF:
	
	    if(!takeoff_cmd_sent){
	
		// Set mode to guided
		SetMode(4);
		apMode = AP_MODE.GUIDED;
		
		// Arm the throttles
		SendCommand(0,0,MAV_CMD.MAV_CMD_COMPONENT_ARM_DISARM,0,
			    1,0,0,0,0,0,0);
		
		// Takeoff at current location
		SendCommand(0,0,MAV_CMD.MAV_CMD_NAV_TAKEOFF,0,
			    1,0,0,0, (float) apState.aircraftPosition.lat,
			    (float) apState.aircraftPosition.lon,
			    (float) apState.aircraftPosition.alt_msl + 50.0f);

		takeoff_cmd_sent = true;

		takeoff_start_time = current_time;
		time1              = takeoff_start_time;
		FP.nextWaypoint = 1;
		
	    }else{
		// Switch to auto once 50 m is reached and start mission in auto mode
		if(apState.aircraftPosition.alt_agl >= 50.0f){
		    missionState = FMS_MISSION.MONITOR;
		    SetMode(3);
		    apMode = AP_MODE.AUTO;

		    // Set speed
		    SendCommand(0,0,MAV_CMD.MAV_CMD_DO_CHANGE_SPEED,0,
			        1,5,0,0,0,0,0);
		}
	    }

	    break;

	case MONITOR:

	    // Check for conflicts and determine mode
	    Monitor();

	    // Set the requested mode
	    if(reqMode != apMode){
		if(apMode == AP_MODE.AUTO){
		    SetMode(3);
		}
		else if(apMode == AP_MODE.GUIDED){
		    SetMode(4);
		}
	    }

	    // Resolve the conflict
	    Resolve();
	    
	    break;

	case LAND:

	    break;
		
	
	}// end switch
    }//end Mission()

    public boolean CheckMissionItemReached(){

	boolean reached = false;
	
	synchronized(SharedData){

	    if(SharedData.RcvdMessages.RcvdMissionItemReached == 1){
		SharedData.RcvdMessages.RcvdMissionItemReached = 0;
		reached = true;
	    }
	}

	return reached;	
    }


    public void Monitor(){

	FlightPlan FP       = SharedData.CurrentFlightPlan;
	Position currentPos = apState.aircraftPosition;
	
	long current_time   = System.nanoTime();
	
	long time_elapsed = current_time - time1;
	
	// Check for deviation from prescribed flight profile.

	// Check for conflicts from DAIDALUS against other traffic.

	// Check for geofence resolutions.

	// Check for mission payload related flags.

	// Check mission progress.
	if(time_elapsed > 5E9){
	    time1 = current_time;    
	    Waypoint wp = FP.GetWaypoint(FP.nextWaypoint);	    
	    double dist[] = FP.Distance2Waypoint(currentPos,wp.pos);	    
	    System.out.format("Distance to next waypoint: %2.2f (Miles), heading: %3.2f (degrees)\n",dist[0]*0.62,dist[1]);	    
	}
	
	if(CheckMissionItemReached()){
	    System.out.println("Reached waypoint");
	    SharedData.CurrentFlightPlan.nextWaypoint++;
	}
	

	// Determine mode
	reqMode = AP_MODE.AUTO;
	
	
    }// end Monitor()


    public void Resolve(){	

	// If conflicts detected, switch to guided mode and perform resolution.
	

    }
}
