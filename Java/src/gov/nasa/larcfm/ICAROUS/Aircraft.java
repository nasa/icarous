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
    public Mission mission;
    
    public FLIGHT_STATE state;
    public long timeStart;
    public long timeCurrent;
    
    private AP_MODE apMode;    // Current AP mode
        
    public Aircraft(Interface ap_Intf,Interface com_Intf,AircraftData acData,Mission mc){
	
	apIntf           = ap_Intf;
	comIntf          = com_Intf;
	FlightData       = acData;
	mission          = mc;
	state            = FLIGHT_STATE.TAKEOFF;
	apMode           = AP_MODE.ND;
	fsam             = new FSAM(this,mission);
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

    public void SetYaw(double heading){

	SendCommand(0,0,MAV_CMD.MAV_CMD_CONDITION_YAW,0,
		    (float)heading,0,1,0,
		    0,0,0);
	
    }

    
    public int SetGPSPos(double lat,double lon, double alt){

	msg_set_position_target_global_int msg= new msg_set_position_target_global_int();

	msg.time_boot_ms     = 0;
	msg.target_system    = 0;
	msg.target_component = 0;
	msg.coordinate_frame = MAV_FRAME.MAV_FRAME_GLOBAL_INT;
	msg.type_mask        = 0b0000111111111000;
	msg.lat_int          = (int) (lat*1E7);
	msg.lon_int          = (int) (lon*1E7);
	msg.alt              = (float) alt;
	msg.vx               = 0;
	msg.vy               = 0;
	msg.vz               = 0;
	msg.afx              = 0;
	msg.afy              = 0;
	msg.afz              = 0;
	msg.yaw              = 0;
	msg.yaw_rate         = 0;

	apIntf.Write(msg);

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

	Position currPosition = FlightData.currPosition;
	timeCurrent           = System.nanoTime();
	FSAM_OUTPUT status;
	
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
	    
	    timeStart          = timeCurrent;
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

	    if(status == FSAM_OUTPUT.CONFLICT){
		fsam.Resolve();
	    }
	    else if(status == FSAM_OUTPUT.NOOP){
		mission.Execute(this);
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
