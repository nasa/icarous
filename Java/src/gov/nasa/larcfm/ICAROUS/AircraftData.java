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
import com.MAVLink.enums.*;
import gov.nasa.larcfm.Util.NavPoint;
import gov.nasa.larcfm.Util.Plan;
import gov.nasa.larcfm.Util.Position;
import gov.nasa.larcfm.Util.Velocity;
import gov.nasa.larcfm.Util.AircraftState;

public class AircraftData{

    public enum FP_WRITE_AP{
	FP_CLR, FP_SEND_COUNT, FP_SEND_WP
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
    
    public AircraftData(){

	Inbox               = new MAVLinkMessages();
	acState             = new AircraftState();
	fenceList           = new ArrayList<GeoFence>();
	obstacles           = new ArrayList<GenericObject>();
	traffic             = new ArrayList<GenericObject>();
	missionObj          = new ArrayList<GenericObject>();
	startMission        = -1;
	FP_nextWaypoint     = 1;
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
	Position P = Position.makeLatLonAlt(lat,lon,alt*3.28);
	
	acState.add(P,V,bootTime);
    }

    public void GetAttitude(){

	msg_attitude msg = Inbox.Attitude();

	roll  = msg.roll*180/Math.PI;
	pitch = msg.pitch*180/Math.PI;
	yaw   = msg.yaw*180/Math.PI;
    }

    // Function to send a flight plan to pixhawk
    public void SendFlightPlanToAP(Interface Intf){

	FP_WRITE_AP state = FP_WRITE_AP.FP_CLR;
	
	msg_mission_count msgMissionCount        = new msg_mission_count();
	msg_mission_item msgMissionItem          = new msg_mission_item();
	msg_mission_clear_all msgMissionClearAll = new msg_mission_clear_all();

	boolean writeComplete = false;
	int count = 0;

	// Copy new flight into current flight plan
	CurrentFlightPlan = new Plan(NewFlightPlan);

	msgMissionCount.target_system    = 0;
	msgMissionCount.target_component = 0;
	msgMissionItem.target_system     = 0;
	msgMissionItem.target_component  = 0;

	msgMissionClearAll.target_system    = 0;
	msgMissionClearAll.target_component = 0;

	synchronized(Intf){
	while(!writeComplete){

	    
		switch(state){

		case FP_CLR:
		    Intf.Write(msgMissionClearAll);
		    //System.out.println("Cleared mission on AP");
		    state = FP_WRITE_AP.FP_SEND_COUNT;
		    count = 0;
		    break;

		case FP_SEND_COUNT:
		
		    msgMissionCount.count = CurrentFlightPlan.size();

		    Intf.Write(msgMissionCount);
		    //System.out.println("Wrote mission count: "+msgMissionCount.count);
		    state = FP_WRITE_AP.FP_SEND_WP;
		    break;
	    
		case FP_SEND_WP:

		    Intf.Read();
		
				
		    if(Inbox.UnreadMissionRequest()){
		    
			Inbox.ReadMissionRequest();

			int seq = Inbox.MissionRequest().seq;
		    
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
			count++;
		    }
		
		    if(Inbox.UnreadMissionAck()){

			Inbox.ReadMissionAck();
		    
			//System.out.println("Received acknowledgement - type: "+Inbox.MissionAck().type);
		    
			if(Inbox.MissionAck().type == 0){
			    if(count == CurrentFlightPlan.size()){
				//System.out.println("Waypoints sent successfully to AP");
				writeComplete = true;
			    }
			
			}
			else{
			    state = FP_WRITE_AP.FP_CLR;
			    //System.out.println("Error in writing mission to AP");
			}
		    }
		} // end of switch case
	    }//end of synchronized
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
		      		      		      		      
		      NewFlightPlan.add(NavPoint.makeLatLonAlt(msg2.latx,msg2.lony,msg2.altz*3.28,count));
		      
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
		
		fenceList.add(fence1);
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
