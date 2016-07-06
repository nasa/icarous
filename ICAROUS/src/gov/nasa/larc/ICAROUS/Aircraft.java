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
import com.MAVLink.icarous.*;

public class Aircraft{

    public AircraftData SharedData;
    public AircraftData apState;
    public ICAROUS_Interface Intf;

    public enum FP_READ_COM{
	FP_INFO, FP_WAYPT_INFO, FP_ACK_FAIL,FP_ACK_SUCCESS
    }

    public enum FP_READ_AP{

    }

    public enum FP_WRITE_AP{
	FP_CLR, FP_SEND_COUNT, FP_SEND_WP
    }

    
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
	    }
	}	
    }
    
    public void GetFlightPlanFromAP(){

	int state = 0;
	msg_mission_request_list msgRequestMissionList = new msg_mission_request_list();
	msg_mission_request msgMissionRequest          = new msg_mission_request();
	boolean readComplete = false;
	
	while(!readComplete){
	    if(state == 0){
		
		msgRequestMissionList.target_system    = 0;
		msgRequestMissionList.target_component = 0;
		
		Intf.Write(msgRequestMissionList);
		state = 1;
	    }

	    if(state == 1){
		synchronized(SharedData){
		    if (SharedData.RcvdMessages.RcvdMissionCount == 1){
			System.out.println("Received missioncount");
			SharedData.RcvdMessages.RcvdMissionCount = 0;

			state = 2;
		    }
		    else{
			state = 0;
		    }
		}
	    }

	    if(state == 2){
	    
		msgMissionRequest.target_system    = 0;
		msgMissionRequest.target_component = 0;

		for(int i=0;i<SharedData.RcvdMessages.msgMissionCount.count;i++){
		    msgMissionRequest.seq          = i;

		    Intf.Write(msgMissionRequest);
		
		    try{
			Thread.sleep(100);
		    }
		    catch(InterruptedException e){
			System.out.println(e);
		    }

		    synchronized(SharedData){
			if (SharedData.RcvdMessages.RcvdMissionItem == 1){
			    System.out.println("Received mission item: "+i);
			    SharedData.RcvdMessages.RcvdMissionItem = 0;

			    System.out.println(SharedData.RcvdMessages.msgMissionItem.x);
			    System.out.println(SharedData.RcvdMessages.msgMissionItem.y);
			    System.out.println(SharedData.RcvdMessages.msgMissionItem.z);
		 
			}
			else{
			    System.out.println("Transaction failed");

			}

		    }
		
		    state = 3;
		    
		}

		if (state == 3){

		    msg_mission_ack msgMissionAck = new msg_mission_ack();
		    msgMissionAck.target_system    = 0;
		    msgMissionAck.target_component = 0;
		    msgMissionAck.type             = 0;

		    Intf.Write(msgMissionAck);
		}
	    
	    
	    }
	}

    }

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
		  
		  //wp.id      = msg2.index;
		  //wp.lat     = msg2.lat;
		  //wp.lon     = msg2.lon;
		  //wp.alt_msl = msg2.alt;
		  //wp.heading = msg2.heading;

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
		  
	      }
	  }
	  
    }
}
