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

	
	Waypoint wp1 = new Waypoint();
	wp1.id       = 0;
	wp1.lat      = 37.611865f;
	wp1.lon      = -122.3754350f;
	wp1.alt      = 0.0f;
	wp1.heading  = 0.0f;

	Waypoint wp2 = new Waypoint();
	wp2.id       = 1;
	wp2.lat      = 37.615267f; 
	wp2.lon      = -122.373179f;
	wp2.alt      = 0.0f;
	wp2.heading  = 0.0f;

	SharedData.CurrentFlightPlan.FlightPlanInfo(2,0,0,0);
	SharedData.CurrentFlightPlan.AddWaypoints(0,wp1);
	SharedData.CurrentFlightPlan.AddWaypoints(1,wp2);

	
	
	System.out.println("Starting write");
	synchronized(SharedData){
	    SendFlightPlanToAP();
	}
    }


    public void SendFlightPlanToAP(){
	
	int state = 0;

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

	System.out.println("Clearing mission on AP");
	AP.Write(msgMissionClearAll);
	
	while(!writeComplete){

	    if(state == 0){
		msgMissionCount.count = SharedData.CurrentFlightPlan.numWayPoints;

		AP.Write(msgMissionCount);
		System.out.println("Wrote mission count");
		state++;
	    }
	    else if(state == 1){

		AP.Read();
		
		try{
		    Thread.sleep(500);
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
		    msgMissionItem.param4  = SharedData.CurrentFlightPlan.GetWaypoints(msgMissionItem.seq).heading;
		    msgMissionItem.x       = SharedData.CurrentFlightPlan.GetWaypoints(msgMissionItem.seq).lat;
		    msgMissionItem.y       = SharedData.CurrentFlightPlan.GetWaypoints(msgMissionItem.seq).lon;
		    msgMissionItem.z       = SharedData.CurrentFlightPlan.GetWaypoints(msgMissionItem.seq).alt;
		    
		    AP.Write(msgMissionItem);
		    System.out.println("Wrote mission item");
		}
		
		
		count++;
		
		if(count == SharedData.CurrentFlightPlan.numWayPoints){
		    state++;
		}
		
	    	
		
	    }
	    else if(state == 2){
		
		if(SharedData.RcvdMessages.RcvdMissionAck == 1){
		    SharedData.RcvdMessages.RcvdMissionAck = 0;
		    System.out.println("Received acknowledgement - type: "+SharedData.RcvdMessages.msgMissionAck.type);
		    if(SharedData.RcvdMessages.msgMissionAck.type == 0){
			writeComplete = true;
			System.out.println("Waypoints sent successfully");
		    }
		    else{
			state = 0;
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
		
		AP.Write(msgRequestMissionList);
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

		    AP.Write(msgMissionRequest);
		
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

		    AP.Write(msgMissionAck);
		}
	    
	    
	    }
	}
	

    }
}
