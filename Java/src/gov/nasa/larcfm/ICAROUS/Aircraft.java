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

    public enum FP_READ_COM{
	FP_INFO, FP_WAYPT_INFO, FP_ACK_FAIL,FP_ACK_SUCCESS
    }

    public enum FP_WRITE_AP{
	FP_CLR, FP_SEND_COUNT, FP_SEND_WP
    }

    public enum QUAD_FMS{
	IDLE, MISSION, TERMINATE 
    }

    public enum FMS_MISSION{
	TAKEOFF, MONITOR, LAND
    }
    
    public AircraftData SharedData;
    public AircraftData apState;
    public ICAROUS_Interface Intf;
    public QUAD_FMS fmsState;
    public FMS_MISSION missionState;

    private boolean takeoff_cmd_sent;
    private long takeoff_start_time;
    private long takeoff_time1;
    private long takeoff_time2;
    
    public Aircraft(){
	fmsState         = QUAD_FMS.IDLE;
	missionState     = FMS_MISSION.TAKEOFF;
	takeoff_cmd_sent = false;
    }

    // Function to send commands to pixhawk
    public int SendCommand( int target_system,
			    int target_component,
			    int confirmation,
			    int command,
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

    // Function to send a flight plan to pixhawk
    public void SendFlightPlanToAP(){
	
	FP_WRITE_AP state = FP_WRITE_AP.FP_CLR;
	
	msg_mission_count msgMissionCount = new msg_mission_count();
	msg_mission_item msgMissionItem   = new msg_mission_item();
	msg_mission_clear_all msgMissionClearAll = new msg_mission_clear_all();
	boolean writeComplete = false;
	int count = 0;

	msgMissionCount.target_system    = 0;
	msgMissionCount.target_component = 0;
	msgMissionItem.target_system     = 0;
	msgMissionItem.target_component  = 0;

	msgMissionClearAll.target_system    = 0;
	msgMissionClearAll.target_component = 0;
		
	while(!writeComplete){

	    switch(state){

	    case FP_CLR:
		Intf.Write(msgMissionClearAll);
		System.out.println("Cleared mission on AP");
		state = FP_WRITE_AP.FP_SEND_COUNT;
		
		break;

	    case FP_SEND_COUNT:
		
		msgMissionCount.count = SharedData.CurrentFlightPlan.numWayPoints;

		Intf.Write(msgMissionCount);
		System.out.println("Wrote mission count: "+SharedData.CurrentFlightPlan.numWayPoints);
		state = FP_WRITE_AP.FP_SEND_WP;
		break;
	    
	    case FP_SEND_WP:

		Intf.Read();
		
		try{
		    Thread.sleep(50);
		}
		catch(InterruptedException e){
		    System.out.println(e);
		}
		
		if(SharedData.RcvdMessages.RcvdMissionRequest == 1){
		    SharedData.RcvdMessages.RcvdMissionRequest = 0;

		    System.out.println("Received mission request: "+SharedData.RcvdMessages.msgMissionRequest.seq);
		    
		    msgMissionItem.seq     = SharedData.RcvdMessages.msgMissionRequest.seq;
		    msgMissionItem.frame   = MAV_FRAME.MAV_FRAME_GLOBAL_RELATIVE_ALT;
		    msgMissionItem.command = MAV_CMD.MAV_CMD_NAV_WAYPOINT;
		    msgMissionItem.current = 0;
		    msgMissionItem.autocontinue = 0;
		    msgMissionItem.param1  = 0.0f;
		    msgMissionItem.param2  = 0.0f;
		    msgMissionItem.param3  = 0.0f;
		    msgMissionItem.param4  = SharedData.CurrentFlightPlan.GetWaypoint(msgMissionItem.seq).heading;
		    msgMissionItem.x       = SharedData.CurrentFlightPlan.GetWaypoint(msgMissionItem.seq).lat;
		    msgMissionItem.y       = SharedData.CurrentFlightPlan.GetWaypoint(msgMissionItem.seq).lon;
		    msgMissionItem.z       = SharedData.CurrentFlightPlan.GetWaypoint(msgMissionItem.seq).alt_msl;
		    
		    Intf.Write(msgMissionItem);
		    System.out.println("Wrote mission item");
		    count++;
		}
		
		if(SharedData.RcvdMessages.RcvdMissionAck == 1){
		    SharedData.RcvdMessages.RcvdMissionAck = 0;
		    System.out.println("Received acknowledgement - type: "+SharedData.RcvdMessages.msgMissionAck.type);
		    if(SharedData.RcvdMessages.msgMissionAck.type == 0){
			if(count == SharedData.CurrentFlightPlan.numWayPoints){
			    System.out.println("Waypoints sent successfully");
			    writeComplete = true;
			}
			
		    }
		    else{
			state = FP_WRITE_AP.FP_CLR;
			System.out.println("Error in writing mission to AP");
		    }
		}
	    } // end of switch case
	}//end of while	
    }//end of function

    
    // Function to update new flight plan received from combox
    public void UpdateFlightPlan(){
	
	  boolean getFP        = true;
	  FP_READ_COM state1   = FP_READ_COM.FP_INFO;
	  int count            = 0;
	  
	  
	  while(getFP){
	      switch(state1){
		  
	      case FP_INFO:
		  
		  msg_flightplan_info msg1 = SharedData.RcvdMessages.msgFlightplanInfo;

		  SharedData.NewFlightPlan.FlightPlanInfo(msg1.numWaypoints,msg1.maxHorDev,msg1.maxVerDev,msg1.standoffDist);
		  
		  state1 = FP_READ_COM.FP_WAYPT_INFO;

		  System.out.println("Received flight plan info, Reading " +msg1.numWaypoints+" waypoints");
		  
		  break;
		  
	      case FP_WAYPT_INFO:
		  
		  Intf.Read();

		  msg_pointofinterest msg2 = SharedData.RcvdMessages.msgPointofinterest;

		  if(msg2.id == 0 && msg2.index != count){
		      
		      state1  = FP_READ_COM.FP_ACK_FAIL;
		      break;
		  }

		  Waypoint wp          = new Waypoint(msg2.index,msg2.lat,msg2.lon,msg2.alt,msg2.heading);
		  
		  System.out.println("waypoint:"+count+" lat:"+wp.lat+" lon:"+wp.lon);
		  SharedData.NewFlightPlan.AddWaypoints(count,wp);

		  if(count == (SharedData.NewFlightPlan.numWayPoints-1)){
		      state1  = FP_READ_COM.FP_ACK_SUCCESS;
		      
		  }
		  else{
		      count++;
		      System.out.println("Receiving next waypoint");
		  }
		  
		  break;

	      case FP_ACK_SUCCESS:

		  msg_command_acknowledgement msg3 = new msg_command_acknowledgement();

		  msg3.acktype = 0;
		  		 
		  getFP = false;

		  // Send acknowledgment
		  msg3.value = 1;
		  
		  Intf.Write(msg3);

		  System.out.println("Waypoints received successfully");
		  
		  break;  
		  
	      case FP_ACK_FAIL:

		  msg_command_acknowledgement msg4 = new msg_command_acknowledgement();

		  msg4.acktype = 0;		  
		  
		  getFP = false;

		  // Send acknowledgment
		  msg4.value = 0;
		  
		  Intf.Write(msg4);
		  
		  break;
		  
	      } // end of switch case
	  }// end of while
    }//end of function

    // Main function that runs the mission
    public void Mission(){

		    	    
	long current_time = System.nanoTime();
	long time_elapsed;
	
	switch(missionState){

	case TAKEOFF:
	
	    if(!takeoff_cmd_sent){
	
		// Set mode to guided
		SetMode(4);
		
		// Arm the throttles
		SendCommand(0,0,0,MAV_CMD.MAV_CMD_COMPONENT_ARM_DISARM,
			    1,0,0,0,0,0,0);
		
		// Takeoff at current location
		SendCommand(0,0,0,MAV_CMD.MAV_CMD_NAV_TAKEOFF,
			    1,0,0,0, (float) apState.aircraftPosition.lat,
			    (float) apState.aircraftPosition.lon,
			    (float) apState.aircraftPosition.alt_msl + 50.0f);

		takeoff_cmd_sent = true;

		takeoff_start_time = current_time;
		time1              = takeoff_start_time;
		
	    }else{
		// Switch to auto once 50 m is reached and start mission in auto mode
		if(apState.aircraftPosition.alt_agl >= 50.0f){
		    missionState = FMS_MISSION.MONITOR;
		    SetMode(3);
		}
	    }

	    break;

	case MONITOR:

	    time_elapsed = time1 - current_time;

	    if(time_elapsed > 5E9){
		time1 = current_time;

		
	    }
	    
	    
	    break;

	case LAND:

	    break;
		
	
	}// end switch
    }//end Monitor()
    
}
