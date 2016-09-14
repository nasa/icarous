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

import java.util.*;
import java.lang.*;
import com.MAVLink.common.*;
import com.MAVLink.icarous.*;
import com.MAVLink.ardupilotmega.*;
import com.MAVLink.enums.*;
import gov.nasa.larcfm.Util.NavPoint;
import gov.nasa.larcfm.Util.Plan;
import gov.nasa.larcfm.Util.Position;
import gov.nasa.larcfm.Util.Velocity;
import gov.nasa.larcfm.Util.AircraftState;
import gov.nasa.larcfm.Util.ParameterData;

public class AircraftData{

    public enum FP_WRITE_AP{
	FP_CLR, FP_CLR_ACK, FP_SEND_COUNT, FP_SEND_WP
    }

    public enum FP_READ{
	FP_ITEM_REQUEST, FP_WP_READ, FP_READ_COMPLETE
    }

    public enum GF{
	GF_READ, GF_FETCH, GF_READ_COMPLETE
    }

    public enum FP_READ_COM{
	FP_INFO, FP_WAYPT_INFO, FP_ACK_FAIL,FP_ACK_SUCCESS
    }

    enum FENCE_STATE{
	    IDLE,INFO,VERTICES,ACK_FAIL,ACK_SUCCESS,UPDATE,REMOVE
    }
    
    public MAVLinkMessages Inbox;
    
    // Aircraft attitude
    public double roll;
    public double pitch;
    public double yaw;
    public double heading;

    // Aircraft's current position (GPS)
    public AircraftState acState;
    
    //public FlightPlan NewFlightPlan;
    //public FlightPlan CurrentFlightPlan;
    public Plan NewFlightPlan;
    public Plan CurrentFlightPlan;
    
    public List<GeoFence> fenceList; 

    public List<msg_mission_item>InputFlightPlan;
    // List for various objects
    public List<GenericObject> obstacles;
    public List<GenericObject> traffic;
    public List<GenericObject> missionObj;
    
    // List for traffic information

    // Flight plan specific data
    public double FP_maxHorizontalDeviation;
    public double FP_maxVerticalDeviation;
    public double FP_standoffDistance;
    public int FP_numWaypoints;
    public int FP_nextWaypoint;

    public int startMission; // -1: last command executed, 0 - stop mission, 1 - start mission

    public double planTime;

    public ParameterData pData;

    private boolean pauseDAQ;
    
    public AircraftData(ParameterData pdata){

	Inbox               = new MAVLinkMessages();
	acState             = new AircraftState();
	fenceList           = new ArrayList<GeoFence>();
	obstacles           = new ArrayList<GenericObject>();
	traffic             = new ArrayList<GenericObject>();
	missionObj          = new ArrayList<GenericObject>();
        InputFlightPlan     = new ArrayList<msg_mission_item>();
	startMission        = -1;
	FP_nextWaypoint     = 0;
	pData               = pdata;
	pauseDAQ            = false;
    }

    public synchronized void SetPauseDAQ(boolean val){
	pauseDAQ =  val;
    }

    public synchronized boolean GetPauseDAQ(){
	return pauseDAQ;
    }

    public void GetGPSdata(){

	double lat,lon,alt;
	double vx,vy,vz;
	double bootTime;
	
	msg_global_position_int GPS = Inbox.GetGlobalPositionInt();
	bootTime = (double) (GPS.time_boot_ms)/1E6;
	lat      = (double) (GPS.lat)/1.0E7;
	lon      = (double) (GPS.lon)/1.0E7;
	alt      = (double) (GPS.relative_alt)/1.0E3;
        vx       = (double) (GPS.vx)/1E2;
	vy       = (double) (GPS.vy)/1E2;
	vz       = (double) (GPS.vz)/1E2;

	Velocity V = Velocity.makeVxyz(vy,vx,"m/s",vz,"m/s");
	Position P = Position.makeLatLonAlt(lat,"degree",lon,"degree",alt,"m");
	
	acState.add(P,V,bootTime);
    }

    public void GetAttitude(){

	msg_attitude msg = Inbox.GetAttitude();

	roll  = msg.roll*180/Math.PI;
	pitch = msg.pitch*180/Math.PI;
	yaw   = msg.yaw*180/Math.PI;


	heading = acState.velocityLast().track("degree");

	if(heading < 0){
	    heading = 360 + heading;
	}
	
    }

    public void getPlanTime(){
	Plan FP = CurrentFlightPlan;

	Position pos = acState.positionLast();
	double legDistance    = FP.pathDistance(FP_nextWaypoint - 1);
	double legTime        = FP.getTime(FP_nextWaypoint) - FP.getTime(FP_nextWaypoint-1);
	double lastWPDistance = FP.point(FP_nextWaypoint-1).position().distanceH(pos);
	double currentTime    = FP.getTime(FP_nextWaypoint-1) + legTime/legDistance * lastWPDistance;

	planTime = currentTime;
    }

    
    // Function to send a flight plan to pixhawk
    public msg_mission_ack SendFlightPlanToAP(Interface Intf){
	SetPauseDAQ(true);
	synchronized(Intf){
	    FP_WRITE_AP state = FP_WRITE_AP.FP_CLR;
	
	    msg_mission_count msgMissionCount        = new msg_mission_count();
	    msg_mission_item msgMissionItem          = new msg_mission_item();
	    msg_mission_clear_all msgMissionClearAll = new msg_mission_clear_all();

	    boolean writeComplete = false;
	    int count = 0;

	    // Make a new Plan using the input mission item
	    CurrentFlightPlan = new Plan();

	    msg_mission_item msgMissionItem0         = new msg_mission_item();
	    for(int i=0;i<InputFlightPlan.size();i++){
		
		msgMissionItem0 = InputFlightPlan.get(i);
		if( (msgMissionItem0.command == MAV_CMD.MAV_CMD_NAV_WAYPOINT) ||
		    (msgMissionItem0.command == MAV_CMD.MAV_CMD_NAV_SPLINE_WAYPOINT) ){
		    double wptime= 0;
		    Position nextWP = Position.makeLatLonAlt(msgMissionItem0.x,"degree",msgMissionItem0.y,"degree",msgMissionItem0.z,"m");
		    if(i > 0 ){

			double vel = msgMissionItem0.param4;

			if(vel < 0.5){
			    vel = 1;
			}
		
			double distance = CurrentFlightPlan.point(i - 1).position().distanceH(nextWP);
			wptime          = CurrentFlightPlan.getTime(i-1) + distance/vel;
			//System.out.println("Times:"+wptime);
		    }		     
	    
		    CurrentFlightPlan.add(new NavPoint(nextWP,wptime));
		}
	    }

	    FP_numWaypoints           = CurrentFlightPlan.size();

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
		    //System.out.println("Cleared mission on AP");
		    state = FP_WRITE_AP.FP_CLR_ACK;
		    count = 0;
		    break;

		case FP_CLR_ACK:
		    
		    Intf.Read();		    
		    
		    msg_mission_ack msgMissionAck1 = Inbox.GetMissionAck();
		    if(msgMissionAck1 != null){
		    		    
			if(msgMissionAck1.type == 0){
			    state = FP_WRITE_AP.FP_SEND_COUNT;
			    //System.out.println("CLEAR acknowledgement");
			}
			else{			  
			    //System.out.println("No CLEAR acknowledgement");
			}
		    }else{
			state = FP_WRITE_AP.FP_SEND_COUNT;
		    }

		    break;

		case FP_SEND_COUNT:
		
		    msgMissionCount.count = CurrentFlightPlan.size();

		    Intf.Write(msgMissionCount);
		    //System.out.println("Wrote mission count: "+msgMissionCount.count);
		    state = FP_WRITE_AP.FP_SEND_WP;
		    break;
	    
		case FP_SEND_WP:

		    Intf.Read();		

		    msg_mission_request msgMissionRequest = Inbox.GetMissionRequest();
		    if(msgMissionRequest != null){
		    
			int seq = msgMissionRequest.seq;
			count   = seq;
			
			//System.out.println("Received mission request: "+ seq );

			msgMissionItem = InputFlightPlan.get(seq);
			//msgMissionItem.seq     = seq;
			//msgMissionItem.frame   = MAV_FRAME.MAV_FRAME_GLOBAL_RELATIVE_ALT;
			//msgMissionItem.command = MAV_CMD.MAV_CMD_NAV_WAYPOINT;
			//msgMissionItem.current = 0;
			//msgMissionItem.autocontinue = 0;
			//msgMissionItem.param1  = 0.0f;
			//msgMissionItem.param2  = 0.0f;
			//msgMissionItem.param3  = 0.0f;
			//msgMissionItem.param4  = 0.0f;
			//msgMissionItem.x       = (float)CurrentFlightPlan.point(msgMissionItem.seq).lla().latitude();
			//msgMissionItem.y       = (float)CurrentFlightPlan.point(msgMissionItem.seq).lla().longitude();
			//msgMissionItem.z       = (float)CurrentFlightPlan.point(msgMissionItem.seq).lla().alt();
						
			Intf.Write(msgMissionItem);
			//System.out.println("Wrote mission item:"+count);
			//System.out.format("lat, lon, alt: %f,%f,%f\n",msgMissionItem.x,msgMissionItem.y,msgMissionItem.z);
			
		    }

		    msg_mission_ack msgMissionAck2 = Inbox.GetMissionAck();
		    if(msgMissionAck2 != null){

			//System.out.println("Received acknowledgement - type: "+Inbox.MissionAck().type);
		    
			if(msgMissionAck2.type == 0){
			    if(count == CurrentFlightPlan.size() - 1){
				//System.out.println("Waypoints sent successfully to AP");
				writeComplete = true;
				SetPauseDAQ(false);
				return msgMissionAck2;
			    }
			
			}
			else if(msgMissionAck2.type == 13){

			}
			else{
			    state = FP_WRITE_AP.FP_CLR;
			    //System.out.println("Error in writing mission to AP");
			}
		    }
		} // end of switch case
	    }//end of while
	} // end of synchronization
	SetPauseDAQ(false);

	return null;
    }//end of function

    // Function to get flight plan
    public int GetWaypoints(Interface Intf,int target_system,int target_component,int COUNT,List<msg_mission_item> mission){

	mission.clear();
	boolean readComplete = false;
	int count = 0;
	
	msg_mission_request msgMissionRequest = new msg_mission_request();

	msgMissionRequest.sysid            = 1;
	msgMissionRequest.compid           = 1;
	msgMissionRequest.target_system    = (short) target_system;
	msgMissionRequest.target_component = (short) target_component;

	msg_mission_ack msgMissionAck = new msg_mission_ack();

	msgMissionAck.sysid            = 1;
	msgMissionAck.compid           = 1;
	msgMissionAck.target_system    = (short)target_system;
	msgMissionAck.target_component = (short)target_component;

	FP_READ state = FP_READ.FP_ITEM_REQUEST;

	double time_start = (float)System.nanoTime()/1E9;
	
	while(!readComplete){

	    double time_now = (float)System.nanoTime()/1E9;

	    if(time_now - time_start > 1){
		return 0;
	    }
	    
		switch(state){

		case FP_ITEM_REQUEST:
		    
		    msgMissionRequest.seq = count;
		    Intf.Write(msgMissionRequest);
		    //System.out.println("wrote count:"+count);
		    state = FP_READ.FP_WP_READ;
		    break;

		case FP_WP_READ:
		    
		    Intf.Read();		    

		    msg_mission_item msgMissionItem = Inbox.GetMissionItem();
		    if(msgMissionItem != null){
			
			//System.out.println("mission sequence received:"+msgMissionItem.seq);
			if(msgMissionItem.seq == count){
			    mission.add(msgMissionItem);
			    state = FP_READ.FP_ITEM_REQUEST;

			    count++;

			    if(count >= COUNT){
				state = FP_READ.FP_READ_COMPLETE;
			    }
			    //System.out.println("Added waypoint");
			    time_start = (float)System.nanoTime()/1E9;
			}
			else{
			    msgMissionAck.type = MAV_MISSION_RESULT.MAV_MISSION_INVALID_SEQUENCE;
			    Intf.Write(msgMissionAck);
			    //System.out.println("Error receiving waypoints");
			    return 0;
			}
		    }

		    break;

		case FP_READ_COMPLETE:

		    msgMissionAck.type = MAV_MISSION_RESULT.MAV_MISSION_ACCEPTED;
		    Intf.Write(msgMissionAck);
		    readComplete = true;
		    //System.out.println("All waypoints received");
		    return 1;
		    		    
		} // end of switch case
	}//end of while
	return 0;
    }//end of function

    public void SendWaypoints(Interface Intf,List<msg_mission_item> mission){

	FP_WRITE_AP state = FP_WRITE_AP.FP_SEND_COUNT;
	
	msg_mission_count msgMissionCount        = new msg_mission_count();
	msg_mission_item msgMissionItem          = new msg_mission_item();

	boolean writeComplete = false;
	int count = 0;	

	msgMissionCount.sysid            = 1;
	msgMissionCount.compid           = 1;
	msgMissionCount.target_system    = 0;
	msgMissionCount.target_component = 0;

	
	msgMissionItem.target_system     = 0;
	msgMissionItem.target_component  = 0;

	while(!writeComplete){
	    
		switch(state){		

		case FP_SEND_COUNT:
		
		    msgMissionCount.count = mission.size();

		    Intf.Write(msgMissionCount);
		    System.out.println("Wrote mission count: "+msgMissionCount.count);
		    state = FP_WRITE_AP.FP_SEND_WP;
		    break;
	    
		case FP_SEND_WP:
		   
		    Intf.Read();				   
		    
		    msg_mission_request msgMissionRequest = Inbox.GetMissionRequest();
		    if(msgMissionRequest != null){
		    
			int seq = msgMissionRequest.seq;
			count   = seq;
		    
			System.out.println("Received mission request: "+ seq );

			msgMissionItem = mission.get(seq);									
			msgMissionItem.sysid   = 1;
			msgMissionItem.compid  = 1;
			
			Intf.Write(msgMissionItem);
			System.out.println("Wrote mission item:"+count);
			//System.out.format("lat, lon, alt: %f,%f,%f\n",msgMissionItem.x,msgMissionItem.y,msgMissionItem.z);
			
		    }

		    msg_mission_ack msgMissionAck = Inbox.GetMissionAck();
		    if(msgMissionAck != null){
		    
			System.out.println("Received acknowledgement - type: "+msgMissionAck.type);
		    
			if(msgMissionAck.type == 0){
			    if(count == mission.size() - 1){
				System.out.println("Waypoints sent successfully to AP");
				writeComplete = true;
			    }
			
			}
			else if(msgMissionAck.type == 13){

			}
			else{
			    // state = FP_WRITE_AP.FP_CLR;
			    //System.out.println("Error in writing mission to AP");
			}
		    }

		    if(count == mission.size() - 1){
			writeComplete = true;
		    }
		    
		} // end of switch case
	}//end of while	
    }//end of function

    
    // Function to get flight plan
    public int GetGeoFence(Interface Intf,msg_command_long msg){

	boolean readComplete = false;
	int count = 0;
	int COUNT = (int) msg.param4;

	
	
	GeoFence fence1 = new GeoFence((int)msg.param2,(int)msg.param3,(int)msg.param4,msg.param5,msg.param6,pData);

	if(COUNT < 1){
	    if( (fence1.ID + 1) <= fenceList.size() ){
		fenceList.remove(fence1.ID);	
	    }
	    return 1;
	}
	
	msg_fence_fetch_point msgFenceFetchPoint = new msg_fence_fetch_point();
	
	msgFenceFetchPoint.sysid            = 1;
	msgFenceFetchPoint.compid           = 1;
	msgFenceFetchPoint.target_system    = (short) msg.sysid;
	msgFenceFetchPoint.target_component = (short) msg.compid;

	msg_fence_status msgFenceStatus = new msg_fence_status();

	msgFenceStatus.sysid            = 2;
	msgFenceStatus.compid           = 0;
	
	GF state = GF.GF_FETCH;

	ArrayList<msg_fence_point> InputFence = new ArrayList<msg_fence_point>();

	long time1 = System.nanoTime();
	
	while(!readComplete){

	    long time2 = System.nanoTime();

	    if(time2 - time1 > 3E9){
		break;
	    }
	    
	    switch(state){

	    case GF_FETCH:

		msgFenceFetchPoint.idx = (byte)count;		    		    
		Intf.Write(msgFenceFetchPoint);					    		
		//System.out.println("Wrote count:"+count);
		state = GF.GF_READ;	       
		
		break;
		
	    case GF_READ:
		
		Intf.Read();								
		
		msg_fence_point msgFencePoint = Inbox.GetFencePoint();
		if(msgFencePoint != null){
			
		    //System.out.println("geofence point received:"+msgFencePoint.idx);
		    if(msgFencePoint.idx == count){

			InputFence.add(msgFencePoint);
			fence1.AddVertex(msgFencePoint.idx,msgFencePoint.lat,msgFencePoint.lng);			    
			    
			count++;
			time1 = time2;
			state = GF.GF_FETCH;    	       
		    }
		    else{

			// Send failure acknowledgement
			//msg_command_ack msgCommandAck = new msg_command_ack();
		
			//msgCommandAck.result = 0;
			//Intf.Write(msgCommandAck);
			
		    }
		}

		if(count >= COUNT){
		    state = GF.GF_READ_COMPLETE;
		}

		break;

	    case GF_READ_COMPLETE:
		    
		readComplete = true;

		if( fence1.ID >= fenceList.size() ){
		    fenceList.add(fence1);
		}
		else{
		    fenceList.set(fence1.ID,fence1);
		}
		


		msg_command_ack msgCommandAck = new msg_command_ack();
		
		msgCommandAck.result = 1;
		Intf.Write(msgCommandAck);		
		//fence1.print();
		return 1;

		
		    
		    
	    } // end of switch case
	}//end of while

	return 0;
    }//end of function
    
    
}
