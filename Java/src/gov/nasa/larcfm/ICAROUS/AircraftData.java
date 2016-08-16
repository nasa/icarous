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

public class AircraftData{

    public enum FP_WRITE_AP{
	FP_CLR, FP_CLR_ACK, FP_SEND_COUNT, FP_SEND_WP
    }

    public enum FP_READ{
	FP_ITEM_REQUEST, FP_WP_READ, FP_READ_COMPLETE
    }

    public enum GF{
	GF_READ, GF_SEND_BACK, GF_READ_COMPLETE
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

    double planTime;
    
    public AircraftData(){

	Inbox               = new MAVLinkMessages();
	acState             = new AircraftState();
	fenceList           = new ArrayList<GeoFence>();
	obstacles           = new ArrayList<GenericObject>();
	traffic             = new ArrayList<GenericObject>();
	missionObj          = new ArrayList<GenericObject>();
        InputFlightPlan     = new ArrayList<msg_mission_item>();
	startMission        = -1;
	FP_nextWaypoint     = 0;
    }

    public void GetGPSdata(){

	double lat,lon,alt;
	double vx,vy,vz;
	double bootTime;
	
	msg_global_position_int GPS = Inbox.GlobalPositionInt();
	bootTime = (double) (GPS.time_boot_ms)/1E6;
	lat      = (double) (GPS.lat)/1.0E7;
	lon      = (double) (GPS.lon)/1.0E7;
	alt      = (double) (GPS.relative_alt)/1.0E3;
        vx       = (double) (GPS.vx)/1E2;
	vy       = (double) (GPS.vy)/1E2;
	vz       = (double) (GPS.vz)/1E2;

	Velocity V = Velocity.makeVxyz(vx,vy,vz);
	Position P = Position.makeLatLonAlt(lat,"degree",lon,"degree",alt,"m");
	
	acState.add(P,V,bootTime);
    }

    public void GetAttitude(){

	msg_attitude msg = Inbox.Attitude();

	roll  = msg.roll*180/Math.PI;
	pitch = msg.pitch*180/Math.PI;
	yaw   = msg.yaw*180/Math.PI;
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
    public void SendFlightPlanToAP(Interface Intf){

	FP_WRITE_AP state = FP_WRITE_AP.FP_CLR;
	
	msg_mission_count msgMissionCount        = new msg_mission_count();
	msg_mission_item msgMissionItem          = new msg_mission_item();
	msg_mission_clear_all msgMissionClearAll = new msg_mission_clear_all();

	boolean writeComplete = false;
	int count = 0;

	// Make a new Plan using the input mission item
	CurrentFlightPlan = new Plan();
	
	for(int i=0;i<InputFlightPlan.size();i++){
	    msgMissionItem = InputFlightPlan.get(i);
	    
	    double wptime= 0;
	    Position nextWP = Position.makeLatLonAlt(msgMissionItem.x,"degree",msgMissionItem.y,"degree",msgMissionItem.z,"m");
	    if(i > 0 ){			  
		double distance = CurrentFlightPlan.point(i - 1).position().distanceH(nextWP);
		wptime          = CurrentFlightPlan.getTime(i-1) + distance/msgMissionItem.param4;
	    }		     
	    
	    CurrentFlightPlan.add(new NavPoint(nextWP,wptime));
	}

	FP_numWaypoints           = CurrentFlightPlan.size();
	System.out.println("Num waypoints:"+FP_numWaypoints);

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
		    
			msgMissionItem.seq     = seq;
			msgMissionItem.frame   = MAV_FRAME.MAV_FRAME_GLOBAL_RELATIVE_ALT;
			msgMissionItem.command = MAV_CMD.MAV_CMD_NAV_WAYPOINT;
			msgMissionItem.current = 0;
			msgMissionItem.autocontinue = 0;
			msgMissionItem.param1  = 0.0f;
			msgMissionItem.param2  = 0.0f;
			msgMissionItem.param3  = 0.0f;
			msgMissionItem.param4  = 0.0f;
			msgMissionItem.x       = (float)CurrentFlightPlan.point(msgMissionItem.seq).lla().latitude();
			msgMissionItem.y       = (float)CurrentFlightPlan.point(msgMissionItem.seq).lla().longitude();
			msgMissionItem.z       = (float)CurrentFlightPlan.point(msgMissionItem.seq).lla().alt();

			
			
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
    }//end of function

    // Function to get flight plan
    public void GetWaypoints(Interface Intf,int target_system,int target_component,int COUNT,List<msg_mission_item> mission){

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
	
	while(!readComplete){

		switch(state){

		case FP_ITEM_REQUEST:
		    
		    msgMissionRequest.seq = count;
		    Intf.Write(msgMissionRequest);
		    //System.out.println("wrote count:"+count);
		    state = FP_READ.FP_WP_READ;
		    break;

		case FP_WP_READ:

		    Intf.SetTimeout(1000);
		    Intf.Read();
		    Intf.SetTimeout(0);

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
			}
			else{
			    msgMissionAck.type = MAV_MISSION_RESULT.MAV_MISSION_INVALID_SEQUENCE;
			    Intf.Write(msgMissionAck);
			    //System.out.println("Error receiving waypoints");
			    return;
			}
		    }

		    break;

		case FP_READ_COMPLETE:

		    msgMissionAck.type = MAV_MISSION_RESULT.MAV_MISSION_ACCEPTED;
		    Intf.Write(msgMissionAck);
		    readComplete = true;
		    //System.out.println("All waypoints received");
		    break;
		    
		    
		} // end of switch case
	}//end of while	
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
		} // end of switch case
	}//end of while	
    }//end of function

    // Function to update new flight plan received from combox
    public void UpdateFlightPlan(Interface Intf){
	
	  boolean getFP        = true;
	  FP_READ_COM state    = FP_READ_COM.FP_INFO;
	  int count            = 0;

	  NewFlightPlan = new Plan();
	  
	  while(getFP){
	      switch(state){
		  
	      case FP_INFO:
		  
		  msg_flightplan_info msg1 = Inbox.FlightplanInfo();

		  FP_numWaypoints           = msg1.numWaypoints;
		  FP_maxHorizontalDeviation = msg1.maxHorDev;
		  FP_maxVerticalDeviation   = msg1.maxVerDev;
		  FP_standoffDistance       = msg1.standoffDist;
		  
		  state = FP_READ_COM.FP_WAYPT_INFO;

		  //System.out.println("Received flight plan info, Reading " +msg1.numWaypoints+" waypoints");
		  
		  break;
		  
	      case FP_WAYPT_INFO:

		  Intf.SetTimeout(500);
		  Intf.Read();
		  Intf.SetTimeout(0);

		  if(Inbox.UnreadWaypoint()){
		      Inbox.ReadWaypoint();
		      msg_pointofinterest msg2 = Inbox.Pointofinterest();
		      
		      if(msg2.id == 0 && msg2.index != count){
		      
			  state  = FP_READ_COM.FP_ACK_FAIL;
			  break;
		      }
		      		      		      		      		      
		      double wptime= 0;
		      Position nextWP = Position.makeLatLonAlt(msg2.latx,"degree",msg2.lony,"degree",msg2.altz,"m");
		      if(count > 0 ){			  
			  double distance = NewFlightPlan.point(count - 1).position().distanceH(nextWP);
			  wptime          = NewFlightPlan.getTime(count-1) + distance/msg2.vx;			  
		      }		     

		      NewFlightPlan.add(new NavPoint(nextWP,wptime));
		      
		      if(count == (FP_numWaypoints-1)){
			  state  = FP_READ_COM.FP_ACK_SUCCESS;
			  
		      }
		      else{
			  count++;
			  //System.out.format("Size of FlightPlan = %d, Receiving next waypoint\n",NewFlightPlan.size());
		      }
		      
		  break;
		      
		  }
		  else{
		      state  = FP_READ_COM.FP_ACK_FAIL;
		      break;
		  }

		  

	      case FP_ACK_SUCCESS:

		  msg_command_acknowledgement msg3 = new msg_command_acknowledgement();

		  msg3.acktype = 0;
		  		 
		  getFP = false;

		  // Send acknowledgment
		  msg3.value = 1;
		  
		  Intf.Write(msg3);

		  //System.out.println("Waypoints received successfully");
		  
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

    // Function to get flight plan
    public void GetGeoFence(Interface Intf,msg_command_long msg){

	boolean readComplete = false;
	int count = 0;
	int COUNT = (int) msg.param4;

	GeoFence fence1 = new GeoFence((int)msg.param2,(int)msg.param3,(int)msg.param4,msg.param5,msg.param6);
		
	msg_fence_fetch_point msgFenceFetchPoint = new msg_fence_fetch_point();
	
	msgFenceFetchPoint.sysid            = 1;
	msgFenceFetchPoint.compid           = 1;
	msgFenceFetchPoint.target_system    = (short) msg.sysid;
	msgFenceFetchPoint.target_component = (short) msg.compid;

	msg_fence_status msgFenceStatus = new msg_fence_status();

	msgFenceStatus.sysid            = 2;
	msgFenceStatus.compid           = 0;
	
	GF state = GF.GF_READ;

	ArrayList<msg_fence_point> InputFence = new ArrayList<msg_fence_point>();

	long time1 = System.nanoTime();
	
	while(!readComplete){

	    long time2 = System.nanoTime();

	    if(time2 - time1 > 3E9){
		break;
	    }
	    
		switch(state){

		case GF_SEND_BACK:
		    
		    Intf.SetTimeout(1000);
		    Intf.Read();
		    Intf.SetTimeout(0);

		    msgFenceFetchPoint = Inbox.GetFenceFetchPoint();

		    int idx = msgFenceFetchPoint.idx;
		    Intf.Write(InputFence.get(idx));
		    //System.out.println("wrote fence fetch point:"+count);
		    state = GF.GF_READ;

		    if(count >= COUNT){
			state = GF.GF_READ_COMPLETE;
		    }
		    
		    break;

		case GF_READ:

		    Intf.SetTimeout(1000);
		    Intf.Read();
		    Intf.SetTimeout(0);

		    msg_fence_point msgFencePoint = Inbox.GetFencePoint();
		    if(msgFencePoint != null){
			
			System.out.println("geofence point received:"+msgFencePoint.idx);
			if(msgFencePoint.idx == count){

			    InputFence.add(msgFencePoint);
			    fence1.AddVertex(msgFencePoint.idx,msgFencePoint.lat,msgFencePoint.lng);			    
			    state = GF.GF_SEND_BACK;

			    count++;
			    time1 = time2;
			    	       
			}
			else{
			    
			}
		    }

		    break;

		case GF_READ_COMPLETE:
		    
		    readComplete = true;

		    if(fence1.Type == 0){
			fenceList.add(0,fence1);
		    }else{
			fenceList.add(fence1);
		    }

		    
		    fence1.print();
		    break;
		    
		    
		} // end of switch case
	}//end of while	
    }//end of function


    public void GetNewGeoFence(Interface Intf){

	GeoFence fence1 = null;
	int count = 0;
	boolean getfence = true;
	FENCE_STATE state;
	int numFences;
	
	msg_geofence_info msg1 = Inbox.GeofenceInfo();

	if(msg1.msgType == 0){
	    state = FENCE_STATE.INFO;
	    //System.out.println("Adding fence");
	}
	else{
	    state = FENCE_STATE.REMOVE;
	    //System.out.println("Removing fence");
	}
	
	while(getfence){    

	    switch(state){
	    
	    case INFO:
	    
		fence1 = new GeoFence(msg1.fenceID,msg1.fenceType,msg1.numVertices,msg1.fenceFloor,msg1.fenceCeiling);
		//System.out.println("Received geofence information: "+msg1.fenceCeiling);
		state = FENCE_STATE.VERTICES;
		break;

	    case VERTICES:

		Intf.SetTimeout(500);
		Intf.Read();
		Intf.SetTimeout(0);
		
		if(Inbox.UnreadVertex()){
		    msg_pointofinterest msg2 = Inbox.Pointofinterest();
		    Inbox.ReadVertex();
		    
		    if(msg2.id == 1 && msg2.index != count){
		      
			state  =  FENCE_STATE.ACK_FAIL;
			break;
		    }

		    //System.out.println("Adding vertex :"+count);
		    fence1.AddVertex(msg2.index,msg2.latx,msg2.lony);
		    count++;

		    
		    if(count == fence1.numVertices){
			state = FENCE_STATE.UPDATE;
			break;
		    }
		}

		break;

	    case UPDATE:
		// Always add the keep in fence as the first fence on the list
		// There should only be one keep in fence
		if(fence1.Type == 0){
		    fenceList.add(0,fence1);
		}else{
		    fenceList.add(fence1);
		}
		numFences = fenceList.size();
		GeoFence gf = (GeoFence)fenceList.get(numFences-1);
		//gf.print();				
		state = FENCE_STATE.ACK_SUCCESS;
		
		break;

	    case REMOVE:

		Iterator Itr = fenceList.iterator();

		while(Itr.hasNext()){
		    GeoFence f1 = (GeoFence) Itr.next();

		    if(f1.ID == msg1.fenceID){
			Itr.remove();
			numFences = fenceList.size();
			//System.out.println("Total fences in ICAROUS:"+numFences);
			break;   
		    }
		}

		getfence = false;

		break;

	       
	    case ACK_FAIL:

		msg_command_acknowledgement msg4 = new msg_command_acknowledgement();

		msg4.acktype = 1;		  
		
		getfence = false;
		
		// Send acknowledgment
		msg4.value = 0;
		
		Intf.Write(msg4);
		
		break;

	    case ACK_SUCCESS:

		msg_command_acknowledgement msg5 = new msg_command_acknowledgement();

		msg5.acktype = 1;		  
		
		getfence = false;
		
		// Send acknowledgment
		msg5.value = 1;
		
		Intf.Write(msg5);

		state = FENCE_STATE.UPDATE;
		
		break;
		
	    }//end of switch
	}//end of while
	    
    }//end of function

    
    
}
