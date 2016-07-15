/**
 * Aircraft
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

    
    public enum AP_MODE{
	ND,AUTO,GUIDED
    }

    public enum FLIGHT_STATE{
	TAKEOFF, TAKEOFF_CLIMB, MONITOR, LAND, TERMINATE
    }
    
    public AircraftData FlightData;
    public Interface apIntf;
    public Interface comIntf;

    public FSAM fsam;
    public MISSION mission;
    
    public FLIGHT_STATE state;
    private long timeStart;
    private long timeEvent1;
    
    private AP_MODE apMode;    // Current AP mode
    private AP_MODE reqMode;   // ICAROUS requested mode
        
    public Aircraft(Interface ap_Intf,Interface com_Intf,AircraftData acData){
	missionState     = FMS_MISSION.TAKEOFF;
	takeoff_cmd_sent = false;
	apMode           = AP_MODE.ND;
	reqMode          = AP_MODE.ND;
	stateResolve     = RESOLUTION.NOMINAL;
	apIntf           = ap_Intf;
	comIntf          = com_Intf;
	FlightData       = acData;
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
	
	apIntf.Write(CommandLong);

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

	apIntf.Write(Mode);

	try{
	    Thread.sleep(100);
	}catch(InterruptedException e){
	    System.out.println(e);
	}
	
	return CheckAcknowledgement();	
    }

    // Check acknowledgement
    public int CheckAcknowledgement(){

	if(FlightData.Inbox.CommandAck().result == 0 ){
	    return 1;
	}
	else{
	    System.out.println("Command not accepted\n");
	    return 0;
	}
	
	
    }

    public int PreFlight(){
	
	// Send flight plan to pixhawk
	FlightData.SendFlightPlanToAP(apIntf);
	
	return 1;
    }

    public int Flight(){

	FlightPlan FP         = FlightData.CurrentFlightPlan;
	Position currPosition = FlightData.currPosition;
	long current_time     = System.nanoTime();
	long time_elapsed;
	int status;
	
	switch(state){

	case TAKEOFF:
		
	    // Set mode to guided
	    SetMode(4);
	    apMode = AP_MODE.GUIDED;
	    
	    // Arm the throttles
	    SendCommand(0,0,MAV_CMD.MAV_CMD_COMPONENT_ARM_DISARM,0,
			1,0,0,0,0,0,0);
	    
	    // Takeoff at current location
	    SendCommand(0,0,MAV_CMD.MAV_CMD_NAV_TAKEOFF,0,
			1,0,0,0, (float) currPosition.lat,
			(float) currPosition.lon,
			(float) currPosition.alt_msl + 50.0f);
	    
	    timeStart          = current_time;
	    timeEvent1         = start_time;
	    FP.nextWaypoint    = 1;
	    state = FLIGHT_STATE.TAKEOFF_CLIMB;

	    break;

	case TAKEOFF_CLIMB:

	    // Switch to auto once 50 m is reached and start mission in auto mode
	    if(currPosition.alt_agl >= 50.0f){
		state = FLIGHT_STATE.MONITOR;
		SetMode(3);
		apMode = AP_MODE.AUTO;
		
		// Set speed
		SendCommand(0,0,MAV_CMD.MAV_CMD_DO_CHANGE_SPEED,0,
			    1,5,0,0,0,0,0);
	    }
	    

	    break;

	case MONITOR:

	    // Check for conflicts and determine mode
	    status = fsam.Monitor();

	    if(status == CONFLICT){
		fsam.Resolve()
	    }
	    else if(status == NOOP){
		mission.Execute();
	    }
	    else{

	    }
	   
		
	    break;

	case LAND:

	    break;

	case TERMINATE:
	    return 1;
	
	}// end switch

	return 0;
	
	
    }//end Flight()

    

    public int Terminate(){

	return 1;
    }




}
